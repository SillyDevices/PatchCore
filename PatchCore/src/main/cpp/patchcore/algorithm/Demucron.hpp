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

#ifndef PATCHCORE_DEMUCRON_HPP
#define PATCHCORE_DEMUCRON_HPP

#include <vector>
#include <queue>
#include <algorithm>

// Demucron algorithm: produces stages (levels) of a DAG. Each stage is a set of
// nodes that have in-degree == 0 after removing all previous stages.
// Returns vector of stages; hasCycle is set to true if some nodes remain (cycle).
// Nodes are assumed to be integers in range [0, nNodes-1].
inline std::vector<std::vector<int>> demucron(int nNodes, const std::vector<std::pair<int,int>>& edges) {
    std::vector<std::vector<int>> adj;
    adj.assign(nNodes, {});
    std::vector<int> indeg(nNodes, 0);

    // build adjacency and indegree, ignore out-of-range edges
    for (const auto &e : edges) {
        int u = e.first;
        int v = e.second;
        adj[u].push_back(v);
        indeg[v]++;
    }

    std::vector<char> removed(nNodes, 0);
    int remaining = nNodes;
    std::vector<std::vector<int>> stages;
    stages.reserve((size_t)std::min(nNodes, 8));

    while (remaining > 0) {
        std::vector<int> cur;
        cur.reserve(16);
        for (int i = 0; i < nNodes; ++i) {
            if (!removed[i] && indeg[i] == 0) cur.push_back(i);
        }
        if (cur.empty()) break; // cycle(s) exist among remaining nodes

        // deterministic order inside stage
        std::sort(cur.begin(), cur.end());

        stages.push_back(cur);

        // remove current stage
        for (int u : cur) {
            removed[u] = 1;
            --remaining;
            for (int v : adj[u]) {
                --indeg[v];
            }
        }
    }

    if (remaining > 0) {
        // collect remaining nodes in deterministic order and append as last stage
        std::vector<int> rem;
        rem.reserve(remaining);
        for (int i = 0; i < nNodes; ++i) if (!removed[i]) rem.push_back(i);
        std::sort(rem.begin(), rem.end());
        if (!rem.empty()) stages.push_back(rem);
        //TODO remove in production
        throw std::runtime_error("Demucron: cycle detected in graph");
    }

    return stages;
}

#endif // PATCHCORE_DEMUCRON_HPP
