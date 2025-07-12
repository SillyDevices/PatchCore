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

#ifndef PATCHCORE_ROUTER_HPP
#define PATCHCORE_ROUTER_HPP

#include "Module.hpp"
#include <vector>

class Router {
public:
    Router() = default;
    virtual ~Router() = default;

    void clearModules();
    void addModule(Module *module);

    void envelope();

    void reset();
    void add(ModuleOutput* from, ModuleInput* to);
    void remove(ModuleOutput* from, ModuleInput* to);

    const std::vector<std::pair<ModuleOutput*, ModuleInput*>> getPatches() const;

private:
    void makeInputsAndOutputs();

protected:
    std::unordered_map<std::string, ModuleInput *> inputs = {};
    std::unordered_map<std::string, ModuleOutput *> outputs = {};
    std::unordered_map<ModuleInput*, std::vector<ModuleOutput *>> matrix = {};

    std::vector<Module*> modules;

    std::vector<ModuleInput*> matrix_inputs;
    std::vector<std::vector<ModuleOutput *>> matrix_outputs;

};

#endif //PATCHCORE_ROUTER_HPP
