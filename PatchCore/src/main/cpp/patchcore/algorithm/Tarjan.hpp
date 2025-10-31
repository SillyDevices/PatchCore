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


#ifndef PATCHCORE_TARJAN_HPP
#define PATCHCORE_TARJAN_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>

// Generic Tarjan SCC algorithm working on node values (Node).
// Node may be an integer id (int) or a pointer type (Module*).
// Parameters:
//  - edges: list of directed edges as pairs of Node (from, to)
// Returns: vector of components; each component is vector<Node> containing nodes in that SCC.
// Note: nodes are inferred from endpoints of edges. Isolated vertices not present in edges won't appear.
template<typename Node>
inline std::vector<std::vector<Node>> tarjanSCC(
        const std::vector<std::pair<Node, Node>>& edges)
{
    std::unordered_map<Node, std::vector<Node>> adj;
    adj.reserve(edges.size() * 2 + 16);

    for (const auto& e : edges) {
        const Node& from = e.first;
        const Node& to = e.second;
        auto it = adj.find(from);
        if (it == adj.end()) adj.emplace(from, std::vector<Node>{to});
        else it->second.push_back(to);
        if (adj.find(to) == adj.end()) adj.emplace(to, std::vector<Node>{});
    }

    std::vector<Node> allNodes;
    allNodes.reserve(adj.size());
    for (const auto &kv : adj) allNodes.push_back(kv.first);

    int indexCounter = 0;
    std::unordered_map<Node, int> index;
    std::unordered_map<Node, int> lowlink;
    std::unordered_set<Node> onStack;
    std::vector<Node> S;
    std::vector<std::vector<Node>> sccs;

    std::function<void(const Node&)> strongconnect;
    strongconnect = [&](const Node& v) {
        index[v] = indexCounter;
        lowlink[v] = indexCounter;
        ++indexCounter;
        S.push_back(v);
        onStack.insert(v);

        auto it = adj.find(v);
        const std::vector<Node> empty;
        const std::vector<Node>& neighbors = (it != adj.end()) ? it->second : empty;
        for (const Node& w : neighbors) {
            if (index.find(w) == index.end()) {
                strongconnect(w);
                lowlink[v] = std::min(lowlink[v], lowlink[w]);
            } else if (onStack.find(w) != onStack.end()) {
                lowlink[v] = std::min(lowlink[v], index[w]);
            }
        }

        if (lowlink[v] == index[v]) {
            std::vector<Node> component;
            while (true) {
                Node w = S.back();
                S.pop_back();
                onStack.erase(w);
                component.push_back(w);
                if (w == v) break;
            }
            sccs.push_back(std::move(component));
        }
    };

    for (const Node& v : allNodes) {
        if (index.find(v) == index.end()) strongconnect(v);
    }

    return sccs;
}

#endif //PATCHCORE_TARJAN_HPP
