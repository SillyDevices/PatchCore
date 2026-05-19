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

#include "GraphRouterDiagnostics.hpp"

#include "patchcore/module/router/GraphRouter.hpp"
#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/ModuleInput.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"
#include "PatchCoreLogger.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
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

std::string formatPatchIo(ModuleOutput* output, ModuleInput* input) {
    return output->getModule()->getModuleName() + ":" + output->getName() +
           " -> " +
           input->getModule()->getModuleName() + ":" + input->getName();
}

std::vector<std::string> findEdgeIoDescriptions(
    int fromModuleId,
    int toModuleId,
    const std::vector<std::pair<ModuleOutput*, ModuleInput*>>& patches,
    const std::unordered_map<Module*, int>& moduleToId
) {
    std::vector<std::string> edgeIoDescriptions;
    for (const auto& patch: patches) {
        const auto fromIt = moduleToId.find(patch.first->getModule());
        const auto toIt = moduleToId.find(patch.second->getModule());
        if (fromIt == moduleToId.end() || toIt == moduleToId.end()) {
            continue;
        }

        if (fromIt->second == fromModuleId && toIt->second == toModuleId) {
            edgeIoDescriptions.push_back(formatPatchIo(patch.first, patch.second));
        }
    }

    std::sort(edgeIoDescriptions.begin(), edgeIoDescriptions.end());
    return edgeIoDescriptions;
}

std::vector<std::string> findCycleIoChain(
    const std::vector<int>& cycleModuleIds,
    const std::vector<std::pair<ModuleOutput*, ModuleInput*>>& patches,
    const std::unordered_map<Module*, int>& moduleToId,
    const std::unordered_map<int, Module*>& idToModule
) {
    std::vector<std::string> ioChain;
    if (cycleModuleIds.size() < 2) {
        return ioChain;
    }

    for (size_t index = 0; index + 1 < cycleModuleIds.size(); ++index) {
        const auto fromModuleId = cycleModuleIds[index];
        const auto toModuleId = cycleModuleIds[index + 1];
        auto edgeIoDescriptions = findEdgeIoDescriptions(fromModuleId, toModuleId, patches, moduleToId);

        if (edgeIoDescriptions.empty()) {
            edgeIoDescriptions.push_back(
                idToModule.at(fromModuleId)->getModuleName() +
                " -> " +
                idToModule.at(toModuleId)->getModuleName() +
                " (module edge; no direct patch io found)"
            );
        }

        ioChain.insert(ioChain.end(), edgeIoDescriptions.begin(), edgeIoDescriptions.end());
    }

    return ioChain;
}

std::vector<std::string> findInternalSccIoEdges(
    const std::vector<int>& sccModuleIds,
    const std::vector<std::pair<ModuleOutput*, ModuleInput*>>& patches,
    const std::unordered_map<Module*, int>& moduleToId
) {
    const std::unordered_set<int> sccModuleIdSet(sccModuleIds.begin(), sccModuleIds.end());
    std::vector<std::string> ioEdges;

    for (const auto& patch: patches) {
        const auto fromIt = moduleToId.find(patch.first->getModule());
        const auto toIt = moduleToId.find(patch.second->getModule());
        if (fromIt == moduleToId.end() || toIt == moduleToId.end()) {
            continue;
        }

        if (
            sccModuleIdSet.find(fromIt->second) != sccModuleIdSet.end() &&
            sccModuleIdSet.find(toIt->second) != sccModuleIdSet.end()
        ) {
            ioEdges.push_back(formatPatchIo(patch.first, patch.second));
        }
    }

    std::sort(ioEdges.begin(), ioEdges.end());
    ioEdges.erase(std::unique(ioEdges.begin(), ioEdges.end()), ioEdges.end());
    return ioEdges;
}

std::vector<int> findCycleInScc(
    const std::vector<int>& sccModuleIds,
    const std::vector<std::pair<int, int>>& moduleGraph,
    bool includeSelfEdges
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
            if (includeSelfEdges || edge.first != edge.second) {
                adjacency[edge.first].push_back(edge.second);
            }
        }
    }

    for (auto& adjacencyItem: adjacency) {
        auto& neighbours = adjacencyItem.second;
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

} // namespace

void GraphRouterDiagnostics::logModuleGraph(
    const GraphRouter& router,
    const char* operationName
) {
    std::ostringstream summary;
    summary
        << operationName
        << " parent=" << router.parentModule->getModuleName()
        << " hasLoops=" << (router._containLoops ? "true" : "false")
        << " modules=" << (router.modules.empty() ? 0 : static_cast<int>(router.modules.size()) - 1)
        << " stages=" << router.envelopeStages.size();
    patchcoreLogInfo(kGraphRouterLogTag, summary.str());

    for (size_t stageIndex = 0; stageIndex < router.envelopeStages.size(); ++stageIndex) {
        const auto& stage = router.envelopeStages[stageIndex];
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

void GraphRouterDiagnostics::logModuleGraph(
    const GraphRouter& router,
    const std::vector<std::vector<int>>& stronglyConnectedComponents,
    const std::vector<std::pair<int, int>>& moduleGraph,
    const char* operationName
) {
    logModuleGraph(router, operationName);

    const auto loopDescriptions = describeLoops(
        stronglyConnectedComponents,
        moduleGraph,
        router.patches,
        router.moduleToId,
        router.idToModule,
        operationName
    );
    for (const auto& loopDescription: loopDescriptions) {
        patchcoreLogInfo(kGraphRouterLogTag, loopDescription);
    }
}

std::vector<std::string> GraphRouterDiagnostics::describeLoops(
    const std::vector<std::vector<int>>& stronglyConnectedComponents,
    const std::vector<std::pair<int, int>>& moduleGraph,
    const std::vector<std::pair<ModuleOutput*, ModuleInput*>>& patches,
    const std::unordered_map<Module*, int>& moduleToId,
    const std::unordered_map<int, Module*>& idToModule,
    const char* operationName
) {
    std::vector<std::string> loopDescriptions;
    size_t loopIndex = 0;

    for (const auto& component: stronglyConnectedComponents) {
        auto cycleModuleIds = findCycleInScc(component, moduleGraph, component.size() == 1);
        if (cycleModuleIds.empty() && component.size() > 1) {
            cycleModuleIds = findCycleInScc(component, moduleGraph, true);
        }
        if (cycleModuleIds.empty()) {
            continue;
        }

        std::vector<std::string> componentModuleNames;
        componentModuleNames.reserve(component.size());
        for (const auto moduleId: component) {
            componentModuleNames.push_back(idToModule.at(moduleId)->getModuleName());
        }

        std::vector<std::string> cycleModuleNames;
        cycleModuleNames.reserve(cycleModuleIds.size());
        for (const auto moduleId: cycleModuleIds) {
            cycleModuleNames.push_back(idToModule.at(moduleId)->getModuleName());
        }

        const auto currentLoopIndex = loopIndex++;
        const auto cycleIoChain = findCycleIoChain(cycleModuleIds, patches, moduleToId, idToModule);
        const auto internalSccIoEdges = findInternalSccIoEdges(component, patches, moduleToId);

        std::ostringstream loopHeader;
        loopHeader
            << operationName << " loop[" << currentLoopIndex << "]"
            << " scc=" << formatBracketList(componentModuleNames)
            << " cycle=" << formatArrowChain(cycleModuleNames);
        loopDescriptions.push_back(loopHeader.str());

        loopDescriptions.push_back(
            std::string(operationName) + " loop[" + std::to_string(currentLoopIndex) + "] path:"
        );
        for (const auto& cycleIoEdge: cycleIoChain) {
            loopDescriptions.push_back(
                std::string(operationName) + " loop[" + std::to_string(currentLoopIndex) + "]   " + cycleIoEdge
            );
        }

        if (!internalSccIoEdges.empty() && internalSccIoEdges != cycleIoChain) {
            loopDescriptions.push_back(
                std::string(operationName) + " loop[" + std::to_string(currentLoopIndex) + "] all internal SCC patch edges:"
            );
            for (const auto& internalIoEdge: internalSccIoEdges) {
                loopDescriptions.push_back(
                    std::string(operationName) + " loop[" + std::to_string(currentLoopIndex) + "]   " + internalIoEdge
                );
            }
        }
    }

    return loopDescriptions;
}
