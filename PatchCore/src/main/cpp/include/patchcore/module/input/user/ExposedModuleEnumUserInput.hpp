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

#ifndef PATCHCORE_PROXYMODULEENUMUSERINPUT_HPP
#define PATCHCORE_PROXYMODULEENUMUSERINPUT_HPP

#include "patchcore/module/input/ExposedModuleUserInput.hpp"
#include "patchcore/module/input/user/EnumUserInput.hpp"
#include <string>

class ExposedModuleEnumUserInput: public virtual ExposedModuleUserInput, public EnumUserInput {
public:
    explicit ExposedModuleEnumUserInput(std::string name, EnumUserInput* userInput)
        : ExposedModuleUserInput(userInput), EnumUserInput(name), _enumInput(userInput) {

    };
    virtual ~ExposedModuleEnumUserInput() = default;
public:

    inline void setValue(int value) override {
        _enumInput->setValue(value);
    }

    inline int getValue() override {
        return _enumInput->getValue();
    }

    ExposedModuleUserInput * createExposed(const std::string &withName) override {
        return new ExposedModuleEnumUserInput(withName, this);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("ExposedModuleEnumUserInput does not support createPolyProxy");
    }

protected:
    EnumUserInput *_enumInput;
};

#endif //PATCHCORE_PROXYMODULEENUMUSERINPUT_HPP
