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

#ifndef PATCHCORE_MODULEOUTPUT_HPP
#define PATCHCORE_MODULEOUTPUT_HPP

#include "patchcore/module/output/Output.hpp"
#include <string>

class Module;
class ProxyModuleOutput;

class ModuleOutput: Output {

public:

    ModuleOutput(std::string outputName): name(outputName) {};
    virtual ~ModuleOutput() = default;

    inline const std::string getName() const {
        return name;
    };

public:
    void setModule(Module* module) {
        _module = module;
    };

    Module* getModule() const {
        return _module;
    };

    virtual ProxyModuleOutput* createProxy(const std::string& withName);


private:
    std::string name;
    Module* _module;

public:
    float value = 0.0f;

};

#endif //PATCHCORE_MODULEOUTPUT_HPP
