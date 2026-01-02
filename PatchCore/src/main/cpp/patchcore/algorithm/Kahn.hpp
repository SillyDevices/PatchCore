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

#ifndef PATCHCORE_KAHN_HPP
#define PATCHCORE_KAHN_HPP

#include <vector>
#include <queue>

// Kahn's algorithm for topological sorting on a graph with nodes [0..nNodes-1]
// edges is a list of directed edges (u,v). Returns a vector with a topological
// ordering of nodes

inline std::vector<int> kahn(int nNodes, const std::vector<std::pair<int,int>>& edges) {
    std::vector<std::vector<int>> adj;
    adj.assign(nNodes, {});
    std::vector<int> indeg(nNodes, 0);

    for (const auto &e : edges) {
        int u = e.first;
        int v = e.second;
        if (u >= 0 && u < nNodes && v >= 0 && v < nNodes) {
            adj[u].push_back(v);
            indeg[v]++;
        }
    }

    std::queue<int> q;
    for (int i = 0; i < nNodes; ++i) if (indeg[i] == 0) q.push(i);

    std::vector<int> topo;
    topo.reserve(nNodes);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        topo.push_back(u);
        for (int v : adj[u]) {
            if (--indeg[v] == 0) q.push(v);
        }
    }

    return topo;
}

#endif // PATCHCORE_KAHN_HPP

