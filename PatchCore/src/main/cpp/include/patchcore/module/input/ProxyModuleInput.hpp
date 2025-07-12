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

#ifndef PATCHCORE_PROXYMODULEINPUT_HPP
#define PATCHCORE_PROXYMODULEINPUT_HPP

#include "patchcore/module/input/ModuleInput.hpp"
#include <string>
#include <vector>

//TODO move implementation to .cpp
class ProxyModuleInput: public ModuleInput {
public:
    ProxyModuleInput(const std::string& inputName, ModuleInput* input): ModuleInput(inputName), _input(input) {

    };

    virtual ~ProxyModuleInput() = default;

    void envelope() const{
        _input->value = value;
    }

    [[nodiscard]]
    ModuleInput* getModuleInput() const{
        return _input;
    }

private:
    ModuleInput* _input;

};

#endif //PATCHCORE_PROXYMODULEINPUT_HPP
