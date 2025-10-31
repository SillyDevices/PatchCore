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

#ifndef PATCHCORE_REMOVEUNREACHABLENODES_HPP
#define PATCHCORE_REMOVEUNREACHABLENODES_HPP

#include <vector>
#include <queue>

inline std::vector<std::pair<int, int>> removeUnreachableNodes(
        const std::vector<std::pair<int, int>>& edges,
        const std::vector<int>& outputNodes)
{
    //find max id from edges
    int maxId = 0;
    for (const auto &e : edges) {
        if (e.first >= maxId) maxId = e.first;
        if (e.second >= maxId) maxId = e.second;
    }
    maxId += 1; //ids are 0-based

    // build adjacency list (incoming connections) for module ids
    std::vector<std::vector<int>> incoming(maxId);
    for (const auto &e : edges) {
        incoming[e.second].push_back(e.first);
    }

    // reverse BFS/DFS from all outputModuleIds to find nodes that can reach an output
    std::vector<int> reachable(maxId, 0);
    std::queue<int> workQueue;
    for (int outId : outputNodes) {
        if (!reachable[outId]) {
            reachable[outId] = 1;
            workQueue.push(outId);
        }
    }
    while (!workQueue.empty()) {
        int cur = workQueue.front(); workQueue.pop();
        for (int src : incoming[cur]) {
            if (!reachable[src]) {
                reachable[src] = 1;
                workQueue.push(src);
            }
        }
    }

    // create filtered graph with only edges between reachable nodes (these edges lie on paths to outputs)
    std::vector<std::pair<int,int>> filteredModuleGraph;
    filteredModuleGraph.reserve(edges.size());
    for (const auto &e : edges) {
        int from = e.first; int to = e.second;
        if (reachable[from] && reachable[to]) {
            filteredModuleGraph.emplace_back(from, to);
        }
    }

    return filteredModuleGraph;
}

#endif //PATCHCORE_REMOVEUNREACHABLENODES_HPP
