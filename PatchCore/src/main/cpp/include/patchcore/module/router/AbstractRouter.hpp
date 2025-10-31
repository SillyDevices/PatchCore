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

#ifndef PATCHCORE_ABSTRACTROUTER_HPP
#define PATCHCORE_ABSTRACTROUTER_HPP

#include "../Module.hpp"

class AbstractRouter {
public:
    virtual ~AbstractRouter() = default;

    virtual void addModule(Module *module) = 0;

    virtual void removeModule(Module *module) = 0;

    virtual void clearModules() = 0;

    //TODO rename to addPatch/removePatch/clearPatches
    virtual void add(ModuleOutput *from, ModuleInput *to) = 0;

    virtual void remove(ModuleOutput *from, ModuleInput *to) = 0;

    virtual void reset() = 0;

    [[nodiscard]]
    virtual std::vector<std::pair<ModuleOutput *, ModuleInput *>> getPatches() const = 0;

    virtual void onStartBuffer(int size) = 0;
    virtual void envelope() = 0;
};

#endif //PATCHCORE_ABSTRACTROUTER_HPP
