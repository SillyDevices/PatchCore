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

#ifndef PATCHCORE_POLYPROXYPATCHMODULE_HPP
#define PATCHCORE_POLYPROXYPATCHMODULE_HPP

#include "PolyProxyModule.hpp"
#include "PatchModule.hpp"
#include "PolyModule.hpp"

/*
 * PolyProxyPatchModule is a proxy for a PatchModule inside a PolyModule.
 * it must have an methods from PatchModule, but PolyModule do almost the same and derives from PatchModule.
 * So to avoid code duplication PolyProxyPatchModule derives from PolyModule
 * It doesn't use any PolyProxyModule methods, but it's needed to be used as a PolyProxyModule
 */

class PolyProxyPatchModule: public PolyProxyModule, public PolyModule {
public:
    PolyProxyPatchModule(ModuleFactory *factory, const PatchModule* module, PolyModule* polyModule);
    ~PolyProxyPatchModule() override = default;
public:
    void envelope() override {
        throw std::runtime_error("PolyProxyPatchModule does not implement envelope method");
    }
// Module interface
// all methods below just call PolyModule versions
public:
    const std::unordered_map<std::string, ModuleInput *>& getModuleInputs() const override;
    ModuleInput* getModuleInput(const std::string& moduleName) override;

    const std::unordered_map<std::string, ModuleOutput *>& getModuleOutputs() const override;
    ModuleOutput * getModuleOutput(const std::string& outputName) override;

    const std::unordered_map<std::string, UserInput *>& getUserInputs() const override;
    UserInput *getUserInput(const std::string& inputName) override;
};

#endif //PATCHCORE_POLYPROXYPATCHMODULE_HPP
