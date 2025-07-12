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

#ifndef PATCHCORE_POLYPROXYMODULE_HPP
#define PATCHCORE_POLYPROXYMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/PolyProxyInput.hpp"
#include "patchcore/module/output/PolyProxyOutput.hpp"
#include "patchcore/module/input/PolyProxyUserInput.hpp"

class PolyModule;

class PolyProxyModule : public Module {
public:
    PolyProxyModule(const Module* module, PolyModule* polyModule);
    virtual ~PolyProxyModule() override = default;

    void envelope() override {
        throw std::runtime_error("PolyProxyModule does not implement envelope method");
    }

    std::unique_ptr<Module> clone() const override {
        throw std::runtime_error("PolyProxyModule does not implement clone method");
    }

    //TODO test it!!! maybe it shouldn't throw an error
    std::unique_ptr<PolyProxyModule> createPolyModuleProxy(PolyModule *polyModule) const override {
        throw std::runtime_error("PolyProxyModule does not implement createPolyModuleProxy method");
    }

protected:
    std::vector<Module *> modulesToProxy;

    std::vector<std::unique_ptr<PolyProxyInput>> polyProxyInputs;
    std::vector<std::unique_ptr<PolyProxyOutput>> polyProxyOutputs;
    std::vector<std::unique_ptr<PolyProxyUserInput>> polyProxyUserInputs;

};

#endif //PATCHCORE_POLYPROXYMODULE_HPP
