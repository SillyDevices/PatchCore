/*
 * PatchCore — Modular Synthesizer Engine
 * Copyright (c) 2025 Evgenii Petrov
 *
 * This file is part of PatchCore.
 *
 * PatchCore is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PatchCore is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU AGPL License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Commercial licensing available: contact sillydevices@gmail.com
 */

#include "patchcore/module/router/GraphRouter.hpp"
#include "patchcore/module/output/ExposedModuleOutput.hpp"
#include "PatchCoreLogger.hpp"
#include "../../algorithm/Tarjan.hpp"
#include "../../algorithm/RemoveUnreachableNodes.hpp"
#include "../../algorithm/Demucron.hpp"
#include <algorithm>
#include <functional>
#include <queue>
#include <sstream>
#include <unordered_set>

namespace {

    constexpr const char* kGraphRouterLogTag = "GraphRouter";

    std::string formatBracketList(std::vector<std::string> values) {
        std::sort(values.begin(), values.end());

        std::ostringstream result;
        result << "[";
        for (size_t index = 0; index < values.size(); ++index) {
            if (index > 0) {
                result << ", ";
            }
            result << values[index];
        }
        result << "]";
        return result.str();
    }

    std::string formatArrowChain(const std::vector<std::string>& values) {
        std::ostringstream result;
        result << "[";
        for (size_t index = 0; index < values.size(); ++index) {
            if (index > 0) {
                result << " -> ";
            }
            result << values[index];
        }
        result << "]";
        return result.str();
    }

    std::vector<std::string> findCycleIoChain(
        const std::vector<int>& cycleModuleIds,
        const std::vector<std::pair<ModuleOutput*, ModuleInput*>>& patches,
        const std::unordered_map<Module*, int>& moduleToId
    ) {
        std::vector<std::string> ioChain;
        if (cycleModuleIds.size() < 2) {
            return ioChain;
        }

        for (size_t index = 0; index + 1 < cycleModuleIds.size(); ++index) {
            const auto fromModuleId = cycleModuleIds[index];
            const auto toModuleId = cycleModuleIds[index + 1];

            std::vector<std::string> edgeIoDescriptions;
            for (const auto& patch: patches) {
                const auto fromIt = moduleToId.find(patch.first->getModule());
                const auto toIt = moduleToId.find(patch.second->getModule());
                if (fromIt == moduleToId.end() || toIt == moduleToId.end()) {
                    continue;
                }

                if (fromIt->second == fromModuleId && toIt->second == toModuleId) {
                    edgeIoDescriptions.push_back(
                        patch.first->getModule()->getModuleName() + ":" + patch.first->getName() +
                        " -> " +
                        patch.second->getModule()->getModuleName() + ":" + patch.second->getName()
                    );
                }
            }

            std::sort(edgeIoDescriptions.begin(), edgeIoDescriptions.end());
            ioChain.insert(ioChain.end(), edgeIoDescriptions.begin(), edgeIoDescriptions.end());
        }

        return ioChain;
    }

    std::vector<int> findCycleInScc(
        const std::vector<int>& sccModuleIds,
        const std::vector<std::pair<int, int>>& moduleGraph
    ) {
        std::unordered_set<int> sccModuleIdSet(sccModuleIds.begin(), sccModuleIds.end());
        std::unordered_map<int, std::vector<int>> adjacency;
        for (const auto moduleId: sccModuleIds) {
            adjacency[moduleId] = {};
        }

        for (const auto& edge: moduleGraph) {
            if (
                sccModuleIdSet.find(edge.first) != sccModuleIdSet.end() &&
                sccModuleIdSet.find(edge.second) != sccModuleIdSet.end()
            ) {
                adjacency[edge.first].push_back(edge.second);
            }
        }

        for (auto& [moduleId, neighbours]: adjacency) {
            std::sort(neighbours.begin(), neighbours.end());
            neighbours.erase(std::unique(neighbours.begin(), neighbours.end()), neighbours.end());
        }

        auto orderedModuleIds = sccModuleIds;
        std::sort(orderedModuleIds.begin(), orderedModuleIds.end());

        std::unordered_map<int, int> stateByModuleId;
        std::unordered_map<int, size_t> stackIndexByModuleId;
        std::vector<int> dfsStack;
        std::vector<int> cycle;

        std::function<bool(int)> dfs = [&](int moduleId) -> bool {
            stateByModuleId[moduleId] = 1;
            stackIndexByModuleId[moduleId] = dfsStack.size();
            dfsStack.push_back(moduleId);

            for (const auto nextModuleId: adjacency[moduleId]) {
                if (stateByModuleId[nextModuleId] == 0) {
                    if (dfs(nextModuleId)) {
                        return true;
                    }
                } else if (stateByModuleId[nextModuleId] == 1) {
                    cycle.assign(
                        dfsStack.begin() + static_cast<std::ptrdiff_t>(stackIndexByModuleId[nextModuleId]),
                        dfsStack.end()
                    );
                    cycle.push_back(nextModuleId);
                    return true;
                }
            }

            dfsStack.pop_back();
            stackIndexByModuleId.erase(moduleId);
            stateByModuleId[moduleId] = 2;
            return false;
        };

        for (const auto moduleId: orderedModuleIds) {
            if (stateByModuleId[moduleId] == 0 && dfs(moduleId)) {
                break;
            }
        }

        return cycle;
    }

    // hashing function for pair<int, int>
    struct PairHash {
        size_t operator()(const std::pair<int, int> &p) const noexcept {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

} // namespace

GraphRouter::GraphRouter(Module *parent): AbstractRouter(), parentModule(parent) {
    if (parent == nullptr) throw std::runtime_error("GraphRouter::GraphRouter: parent module is nullptr");
    modules.push_back(parent);
    moduleToId[parent] = nextModuleId;
    idToModule[nextModuleId] = parent;
    nextModuleId++;
}

void GraphRouter::addModule(Module *module) {
    modules.push_back(module);
    moduleToId[module] = nextModuleId;
    idToModule[nextModuleId] = module;
    nextModuleId++;
    // add module by itself does not change anything in the graph, so no need to update it
}

void GraphRouter::removeModule(Module *module) {
    // remove module from modules list
    auto moduleIterator = std::remove(modules.begin(), modules.end(), module);
    if (moduleIterator != modules.end()) {
        modules.erase(moduleIterator, modules.end());
    }
    for (auto it = patches.begin(); it != patches.end();) {
        auto& patch = *it;

        if (patch.first->getModule() == module || patch.second->getModule() == module) {
            patch.second->value = patch.second->getDisconnectedValue(); // set disconnected value
            it = patches.erase(it);
        } else {
            ++it;
        }
    }

    // rebuild moduleIds map
    moduleToId.clear();
    for (int i = 0; i < modules.size(); ++i) {
        moduleToId[modules[i]] = i;
        idToModule[i] = modules[i];
    }
    nextModuleId = static_cast<int>(modules.size());

    updateModuleGraph();
}

void GraphRouter::clearModules() {
    for (const auto &patch: patches) {
        patch.second->value = patch.second->getDisconnectedValue(); // set disconnected value
    }
    patches.clear();
    modules.clear();

    modules.push_back(parentModule);

    moduleToId.clear();
    idToModule.clear();
    nextModuleId = 0;
    moduleToId[parentModule] = nextModuleId;
    idToModule[nextModuleId] = parentModule;
    nextModuleId++;

    updateModuleGraph();
}

void GraphRouter::add(ModuleOutput *from, ModuleInput *to) {
    // assert that patch input and output belong to modules managed by this router
    bool fromModuleFound = false;
    for (const auto &module: modules) {
        if (from->getModule() == module || to->getModule() == module) {
            fromModuleFound = true;
            break;
        }
    }
    if (!fromModuleFound) {
        throw std::runtime_error("GraphRouter::add: trying to add a patch( " +
            from->getModule()->getModuleName() + ":"+ from->getName() + " to " +
            to->getModule()->getModuleName() + ":" + to->getName() +
            " ) with modules not managed by this router( "+ parentModule->getModuleName() +" )");
    }
    patchcoreLogInfo(kGraphRouterLogTag, "add patch from " + from->getModule()->getModuleName() + ":"+ from->getName() + " to " + to->getModule()->getModuleName() + ":" + to->getName());
    patches.emplace_back(from, to);
    updateModuleGraph();
}

void GraphRouter::remove(ModuleOutput *from, ModuleInput *to) {
    for (auto it = patches.begin(); it != patches.end(); ) {
        auto& patch = *it;
        if (patch.first == from && patch.second == to) {
            patch.second->value = patch.second->getDisconnectedValue(); // set disconnected value
            it = patches.erase(it);
        } else {
            ++it;
        }
    }

    updateModuleGraph();
}

void GraphRouter::reset() {
    for (const auto &patch: patches) {
        patch.second->value = patch.second->getDisconnectedValue(); // set disconnected value
    }
    patches.clear();
    updateModuleGraph();
}


void GraphRouter::moduleInputChanged(Module* module) {
    //TODO pre compute outputModules, maybe not needed
    updateModuleGraph();
}


void GraphRouter::updateModuleGraph() {
    std::lock_guard<std::mutex> lock(updateModuleGraphMutex);
    bool hasLoops = false;

    int parentModuleId = moduleToId[parentModule];
    //create connection graph with ids

    std::unordered_set<int> byProxyOutputModuleIds;
    std::unordered_set<int> outputModuleIds;

    for (auto &module: modules) {
        if (module->hasProxyOutputs()) {
            byProxyOutputModuleIds.insert(moduleToId[module]);
        }
    }

    std::unordered_set<std::pair<int, int>, PairHash> moduleGraphSet;

    for (const auto &patch: patches) {
        auto fromModuleId = moduleToId[patch.first->getModule()];

        auto toModule = patch.second->getModule();
        auto toModuleId = moduleToId[patch.second->getModule()];

        if (toModule == parentModule) {
            outputModuleIds.insert(toModuleId);
        }

        if (toModule->needEnvelopeOnInputConnection()) {
            outputModuleIds.insert(toModuleId);
        }

        patchcoreLogInfo(kGraphRouterLogTag, "updateModuleGraph add edge from " + patch.first->getModule()->getModuleName() + ":"+ patch.first->getName() + " to " + patch.second->getModule()->getModuleName() + ":" + patch.second->getName());
        moduleGraphSet.insert({ fromModuleId, toModuleId } );
    }

    //add edges from modules with proxy outputs to parent output node
    for (const auto &moduleId: byProxyOutputModuleIds) {
        moduleGraphSet.insert({ moduleId, parentModuleId });
    }

    //outputModuleIds.insert(byProxyOutputModuleIds.begin(), byProxyOutputModuleIds.end());
    outputModuleIds.insert(parentModuleId);

    std::vector<std::pair<int, int>> moduleGraph = std::vector(moduleGraphSet.begin(), moduleGraphSet.end());

    // mo modules in graph; reset connection matrix and update
    if (moduleGraph.empty()) {
        envelopeStages.clear();
        logModuleGraph("updateModuleGraph");
        return;
    }

    // prune unreachable nodes from graph
    auto outputIds = std::vector<int>(outputModuleIds.begin(), outputModuleIds.end()); // here only parnet module id and modules needing envelope on input connection
    auto filteredModuleGraph = removeUnreachableNodes(moduleGraph, outputIds);
    moduleGraph.swap(filteredModuleGraph);

    // compute strongly connected components (SCCs) of the module graph
    auto SCCs = tarjanSCC<int>(moduleGraph);

    // build condensed graph of SCCs

    //helper map from moduleId to sccId
    std::unordered_map<int, int> moduleIdToSCCId;
    std::vector<std::string> loopDescriptions;
    for (int sccId = 0; sccId < SCCs.size(); ++sccId) {
        for (const auto &moduleId: SCCs[sccId]) {
            moduleIdToSCCId[moduleId] = sccId;
        }
        if (SCCs[sccId].size() > 1) {
            hasLoops = true;
        }

        const auto cycleModuleIds = findCycleInScc(SCCs[sccId], moduleGraph);
        if (!cycleModuleIds.empty()) {
            std::vector<std::string> sccModuleNames;
            sccModuleNames.reserve(SCCs[sccId].size());
            for (const auto moduleId: SCCs[sccId]) {
                sccModuleNames.push_back(idToModule[moduleId]->getModuleName());
            }

            std::vector<std::string> cycleModuleNames;
            cycleModuleNames.reserve(cycleModuleIds.size());
            for (const auto moduleId: cycleModuleIds) {
                cycleModuleNames.push_back(idToModule[moduleId]->getModuleName());
            }

            const auto cycleIoChain = findCycleIoChain(cycleModuleIds, patches, moduleToId);

            std::ostringstream loopDescription;
            loopDescription
                << "updateModuleGraph loop[" << loopDescriptions.size() << "]"
                << " scc=" << formatBracketList(sccModuleNames)
                << " chain=" << formatArrowChain(cycleModuleNames)
                << " io=" << formatBracketList(cycleIoChain);
            loopDescriptions.push_back(loopDescription.str());
        }
    }

    std::vector<std::pair<int, int>> condensedGraph;
    condensedGraph.reserve(moduleGraph.size());

    // build edges between SCCs with removed self-loops and duplicate edges
    for (const auto &edge: moduleGraph) {
        int fromSCC = moduleIdToSCCId[edge.first];
        int toSCC = moduleIdToSCCId[edge.second];

        if (fromSCC == toSCC) {
            //TODO mark as self loop for envelope stage
            hasLoops = true;
            continue; // remove self-loops from condensed graph
        }

        bool found = false;
        for (const auto &e: condensedGraph) {
            if (e.first == fromSCC && e.second == toSCC) {
                found = true;
                break;
            }
        }
        if (!found) {
            condensedGraph.emplace_back(fromSCC, toSCC);
        }
    }

    // IMPLEMENTATION with demucron
    // topological sort of condensed graph
    int K = static_cast<int>(SCCs.size());
    auto stagedSCCTopo = demucron(K, condensedGraph);

    // build envelope stages with topological order of SCCs
    //    - create adjacency list for incoming patches to each module
    std::unordered_map<int, std::vector<std::pair<ModuleOutput *, ModuleInput *>>> incomingPatches;
    for (const auto &patch: patches) {
        auto toModuleId = moduleToId[patch.second->getModule()];
        incomingPatches[toModuleId].emplace_back(patch.first, patch.second);
    }

    std::unordered_set<FloatUserInput*> envelopeUserInputSet;
    std::unordered_set<ModuleInput*> envelopeInputSet;
    std::unordered_set<Module*> envelopeModuleSet;
    envelopeStages.clear();
    for (const auto &sccTopo: stagedSCCTopo) {
        EnvelopeStage stage;
        for (const auto &sccId: sccTopo) {
            // add modules in this SCC to the stage
            stage.modulesInStage.reserve(SCCs[sccId].size());
            std::unordered_map<ModuleInput *, std::vector<ModuleOutput *>> stageMatrix = {};
            for (const auto &moduleId: SCCs[sccId]) {
                if (moduleId != parentModuleId) {
                    const auto module = idToModule[moduleId];
                    stage.modulesInStage.push_back(module);
                    // collect user inputs to envelope
                    auto userInputs = module->getInterpolatedInputs();
                    for (auto userInput: userInputs) {
                        envelopeUserInputSet.insert(userInput);
                    }
                    // collect modules to envelope
                    envelopeModuleSet.insert(module);
                }
                // find input patches to this module from pathes
                for (const auto &patch: incomingPatches[moduleId]) {
                    stageMatrix[patch.second].push_back(patch.first);
                    envelopeInputSet.insert(patch.second);
                }
            }
            // convert stageMatrix to inputsInStage and outputsInStage
            for (const auto &matrixKV: stageMatrix) {
                stage.inputsInStage.push_back(matrixKV.first);
                stage.outputsInStage.push_back(matrixKV.second);
            }
        }
        envelopeStages.push_back(stage);
    }

    allEnvelopeInputs.clear();
    //allEnvelopeInputs.insert(envelopeUserInputSet.begin(), envelopeUserInputSet.end());
    for (const auto &input: envelopeInputSet) {
        allEnvelopeInputs.push_back(input);
    }

    allEnvelopeUserInputs = std::vector<FloatUserInput*>(envelopeUserInputSet.begin(), envelopeUserInputSet.end());

    allEnvelopeModules = std::vector<Module*>(envelopeModuleSet.begin(), envelopeModuleSet.end());

    _containLoops = hasLoops;
    logModuleGraph("updateModuleGraph");
    for (const auto& loopDescription: loopDescriptions) {
        patchcoreLogInfo(kGraphRouterLogTag, loopDescription);
    }
}

std::vector<std::pair<ModuleOutput *, ModuleInput *>> GraphRouter::getPatches() const {
    return patches;
}

void GraphRouter::logModuleGraph(const char* operationName) const {
    std::ostringstream summary;
    summary
        << operationName
        << " parent=" << parentModule->getModuleName()
        << " hasLoops=" << (_containLoops ? "true" : "false")
        << " modules=" << (modules.empty() ? 0 : static_cast<int>(modules.size()) - 1)
        << " stages=" << envelopeStages.size();
    patchcoreLogInfo(kGraphRouterLogTag, summary.str());

    for (size_t stageIndex = 0; stageIndex < envelopeStages.size(); ++stageIndex) {
        const auto& stage = envelopeStages[stageIndex];
        const auto stagePrefix = std::string("\t stage[") + std::to_string(stageIndex) + "]";

        std::vector<std::string> moduleNames;
        moduleNames.reserve(stage.modulesInStage.size());
        for (const auto* module: stage.modulesInStage) {
            moduleNames.push_back(module->getModuleName());
        }

        patchcoreLogInfo(
            kGraphRouterLogTag,
            stagePrefix + " modules=" + formatBracketList(moduleNames)
        );

        if (stage.inputsInStage.empty()) {
            patchcoreLogInfo(
                kGraphRouterLogTag,
                stagePrefix + " routes=<empty>"
            );
            continue;
        }

        std::vector<std::string> routeLines;
        routeLines.reserve(stage.inputsInStage.size());
        for (size_t inputIndex = 0; inputIndex < stage.inputsInStage.size(); ++inputIndex) {
            const auto* input = stage.inputsInStage[inputIndex];
            std::vector<std::string> outputNames;
            outputNames.reserve(stage.outputsInStage[inputIndex].size());
            for (const auto* output: stage.outputsInStage[inputIndex]) {
                outputNames.push_back(output->getModule()->getModuleName() + ":" + output->getName());
            }

            routeLines.push_back(
                stagePrefix + " route " +
                input->getModule()->getModuleName() + ":" + input->getName() +
                " <- " + formatBracketList(outputNames)
            );
        }

        std::sort(routeLines.begin(), routeLines.end());
        for (const auto& routeLine: routeLines) {
            patchcoreLogInfo(kGraphRouterLogTag, routeLine);
        }
    }
}

void GraphRouter::onStartBuffer(int size) {
    for (const auto &module: allEnvelopeModules) {
        module->onStartBuffer(size);
    }
    for (const auto &userInput: allEnvelopeUserInputs) {
        userInput->onStartBuffer(size);
    }
}

void GraphRouter::envelope() {
    for (const auto &userInput: allEnvelopeUserInputs) {
        userInput->envelope();
    }
    for (const auto &input: allEnvelopeInputs) {
        input->value = 0.0f;
    }
    for (const auto &stage: envelopeStages) {
        int inputIndex = 0;
        for (const auto &input: stage.inputsInStage) {
            for (const auto &output: stage.outputsInStage[inputIndex]) {
                input->value += output->value;
            }
            inputIndex++;
        }
        for (const auto &module: stage.modulesInStage) {
            module->envelope(); //skip for now
        }
    }
}
