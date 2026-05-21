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
#include "GraphRouterDiagnostics.hpp"
#include "../../algorithm/Tarjan.hpp"
#include "../../algorithm/RemoveUnreachableNodes.hpp"
#include "../../algorithm/Demucron.hpp"
#include <algorithm>
#include <cstring>
#include <unordered_set>

#ifndef PATCHCORE_GRAPH_ROUTER_DEBUG
#define PATCHCORE_GRAPH_ROUTER_DEBUG 0
#endif

namespace {
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
            patch.second->value.fill(patch.second->getDisconnectedValue()); // set disconnected value
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
        patch.second->value.fill(patch.second->getDisconnectedValue()); // set disconnected value
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
    patches.emplace_back(from, to);
    updateModuleGraph();
}

void GraphRouter::remove(ModuleOutput *from, ModuleInput *to) {
    for (auto it = patches.begin(); it != patches.end(); ) {
        auto& patch = *it;
        if (patch.first == from && patch.second == to) {
            patch.second->value.fill(patch.second->getDisconnectedValue()); // set disconnected value
            it = patches.erase(it);
        } else {
            ++it;
        }
    }

    updateModuleGraph();
}

void GraphRouter::reset() {
    for (const auto &patch: patches) {
        patch.second->value.fill(patch.second->getDisconnectedValue()); // set disconnected value
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
        allEnvelopeInputs.clear();
        allEnvelopeUserInputs.clear();
        allEnvelopeModules.clear();
        _containLoops = false;
#if PATCHCORE_GRAPH_ROUTER_DEBUG
        GraphRouterDiagnostics::logModuleGraph(*this, "updateModuleGraph");
#endif
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
    std::unordered_set<int> loopSCCIds;
    for (int sccId = 0; sccId < SCCs.size(); ++sccId) {
        for (const auto &moduleId: SCCs[sccId]) {
            moduleIdToSCCId[moduleId] = sccId;
        }
        if (SCCs[sccId].size() > 1) {
            hasLoops = true;
            loopSCCIds.insert(sccId);
        }
    }

    std::vector<std::pair<int, int>> condensedGraph;
    condensedGraph.reserve(moduleGraph.size());

    // build edges between SCCs with removed self-loops and duplicate edges
    for (const auto &edge: moduleGraph) {
        int fromSCC = moduleIdToSCCId[edge.first];
        int toSCC = moduleIdToSCCId[edge.second];

        if (fromSCC == toSCC) {
            hasLoops = true;
            loopSCCIds.insert(fromSCC);
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
        for (const auto &sccId: sccTopo) {
            // Keep each SCC in a separate stage so looped SCCs can be processed sample-by-sample
            // without forcing unrelated acyclic SCCs from the same topological layer into that mode.
            EnvelopeStage stage;
            stage.hasLoop = loopSCCIds.find(sccId) != loopSCCIds.end();
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
            // convert stageMatrix to input/output routes
            for (const auto &matrixKV: stageMatrix) {
                auto* input = matrixKV.first;
                const auto& outputs = matrixKV.second;

                stage.inputsInStage.push_back(input);
                stage.outputsInStage.push_back(outputs);

                if (outputs.size() == 1) {
                    EnvelopeStage::SingleOutputInputRoute route;
                    route.input = input;
                    route.output = outputs.front();
                    stage.singleOutputInputRoutes.push_back(route);
                } else {
                    EnvelopeStage::MultiOutputInputRoute route;
                    route.input = input;
                    route.outputs = outputs;
                    stage.multiOutputInputRoutes.push_back(std::move(route));
                }
            }

            if (stage.hasLoop) {
                std::unordered_map<Module*, int> moduleOrder;
                moduleOrder.reserve(stage.modulesInStage.size());
                for (int moduleIndex = 0; moduleIndex < stage.modulesInStage.size(); ++moduleIndex) {
                    moduleOrder[stage.modulesInStage[moduleIndex]] = moduleIndex;
                }

                stage.sampleModuleRoutes.reserve(stage.modulesInStage.size());
                for (int moduleIndex = 0; moduleIndex < stage.modulesInStage.size(); ++moduleIndex) {
                    auto* module = stage.modulesInStage[moduleIndex];
                    EnvelopeStage::SampleModuleRoute moduleRoute;
                    moduleRoute.module = module;

                    for (int inputIndex = 0; inputIndex < stage.inputsInStage.size(); ++inputIndex) {
                        auto* input = stage.inputsInStage[inputIndex];
                        if (input->getModule() != module) {
                            continue;
                        }

                        EnvelopeStage::SampleInputRoute inputRoute;
                        inputRoute.input = input;
                        inputRoute.outputs = stage.outputsInStage[inputIndex];
                        inputRoute.readPreviousSample.reserve(inputRoute.outputs.size());
                        for (const auto &output: inputRoute.outputs) {
                            auto outputModuleIt = moduleOrder.find(output->getModule());
                            inputRoute.readPreviousSample.push_back(
                                outputModuleIt != moduleOrder.end() && outputModuleIt->second >= moduleIndex
                            );
                        }
                        moduleRoute.inputRoutes.push_back(std::move(inputRoute));
                    }
                    stage.sampleModuleRoutes.push_back(std::move(moduleRoute));
                }
            }

            envelopeStages.push_back(stage);
        }
    }

    allEnvelopeInputs.clear();
    //allEnvelopeInputs.insert(envelopeUserInputSet.begin(), envelopeUserInputSet.end());
    for (const auto &input: envelopeInputSet) {
        allEnvelopeInputs.push_back(input);
    }

    allEnvelopeUserInputs = std::vector<FloatUserInput*>(envelopeUserInputSet.begin(), envelopeUserInputSet.end());

    allEnvelopeModules = std::vector<Module*>(envelopeModuleSet.begin(), envelopeModuleSet.end());

    _containLoops = hasLoops;
#if PATCHCORE_GRAPH_ROUTER_DEBUG
    GraphRouterDiagnostics::logModuleGraph(*this, SCCs, moduleGraph, "updateModuleGraph");
#endif
}

std::vector<std::pair<ModuleOutput *, ModuleInput *>> GraphRouter::getPatches() const {
    return patches;
}

void GraphRouter::onStartBlock(const BlockContext& context) {
    for (const auto &module: allEnvelopeModules) {
        module->onStartBlock(context);
    }
    for (const auto &input: allEnvelopeInputs) {
        input->prepareBlock(context);
    }
    for (const auto &patch: patches) {
        patch.first->prepareBlock(context);
        patch.second->prepareBlock(context);
    }
    for (const auto &userInput: allEnvelopeUserInputs) {
        userInput->prepareBlock(context);
    }
}

void GraphRouter::processSample(int sampleIndex) {
    for (const auto &stage: envelopeStages) {
        if (stage.hasLoop) {
            processStageSampleWithLoop(stage, sampleIndex);
        } else {
            processStageSample(stage, sampleIndex);
        }
    }
}

void GraphRouter::processBlock() {
    for (const auto &stage: envelopeStages) {
        if (stage.hasLoop) {
            for (int sampleIndex = 0; sampleIndex < PATCHCORE_BLOCK_SIZE; ++sampleIndex) {
                processStageSampleWithLoop(stage, sampleIndex);
            }
        } else {
            processStageBlock(stage);
        }
    }
}

void GraphRouter::processStageSampleWithLoop(const EnvelopeStage& stage, int sampleIndex) {
    const int previousSampleIndex = sampleIndex == 0 ? PATCHCORE_BLOCK_SIZE - 1 : sampleIndex - 1;
    for (const auto &moduleRoute: stage.sampleModuleRoutes) {
        for (const auto &inputRoute: moduleRoute.inputRoutes) {
            auto* inputBuffer = inputRoute.input->value.data();
            if (inputBuffer == nullptr) {
                inputRoute.input->clearBlock();
                inputBuffer = inputRoute.input->value.data();
            }
            inputBuffer[sampleIndex] = 0.0f;

            for (int outputIndex = 0; outputIndex < inputRoute.outputs.size(); ++outputIndex) {
                auto* output = inputRoute.outputs[outputIndex];
                const auto* outputBuffer = output->value.data();
                if (outputBuffer == nullptr) {
                    output->clearBlock();
                    outputBuffer = output->value.data();
                }

                const int sourceSampleIndex = inputRoute.readPreviousSample[outputIndex]
                    ? previousSampleIndex
                    : sampleIndex;
                inputBuffer[sampleIndex] += outputBuffer[sourceSampleIndex];
            }
        }
        moduleRoute.module->processSample(sampleIndex);
    }
}

void GraphRouter::processStageSample(const EnvelopeStage& stage, int sampleIndex) {
    int inputIndex = 0;
    for (const auto &input: stage.inputsInStage) {
        auto* inputBuffer = input->value.data();
        if (inputBuffer == nullptr) {
            input->clearBlock();
            inputBuffer = input->value.data();
        }
        inputBuffer[sampleIndex] = 0.0f;

        for (const auto &output: stage.outputsInStage[inputIndex]) {
            const auto* outputBuffer = output->value.data();
            if (outputBuffer == nullptr) {
                output->clearBlock();
                outputBuffer = output->value.data();
            }
            inputBuffer[sampleIndex] += outputBuffer[sampleIndex];
        }
        inputIndex++;
    }
    for (const auto &module: stage.modulesInStage) {
        module->processSample(sampleIndex);
    }
}

void GraphRouter::processStageBlock(const EnvelopeStage& stage) {

    for (const auto &route : stage.singleOutputInputRoutes) {
        float* inputBuffer = route.input->value.data();
        const float* outputBuffer = route.output->value.data();
        std::memcpy(inputBuffer, outputBuffer, PATCHCORE_BLOCK_SIZE * sizeof(float));
    }

    for (const auto &route : stage.multiOutputInputRoutes) {
        float* inputBuffer = route.input->value.data();
        const auto& outputs = route.outputs;

        const float* firstOutput = outputs.front()->value.data();
        for (int s = 0; s < PATCHCORE_BLOCK_SIZE; ++s) {
            inputBuffer[s] = firstOutput[s];
        }

        for (auto it = std::next(outputs.begin()); it != outputs.end(); ++it) {
            const float* outputBuffer = (*it)->value.data();

            for (int s = 0; s < PATCHCORE_BLOCK_SIZE; ++s) {
                inputBuffer[s] += outputBuffer[s];
            }
        }
    }

    for (const auto &module: stage.modulesInStage) {
        module->processBlock();
    }
}
