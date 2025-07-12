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

#ifndef PATCHCORE_PROXYMODULEBOOLUSERINPUT_HPP
#define PATCHCORE_PROXYMODULEBOOLUSERINPUT_HPP

#include "patchcore/module/input/ProxyModuleUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"

#include <string>

class ProxyModuleBoolUserInput: public virtual ProxyModuleUserInput, public BoolUserInput {
public:
    explicit ProxyModuleBoolUserInput(std::string name, BoolUserInput* userInput)
        : ProxyModuleUserInput(userInput), BoolUserInput(name), _boolInput(userInput) {};
    virtual ~ProxyModuleBoolUserInput() = default;
public:

    inline virtual void setValue(bool value) override{
        _boolInput->setValue(value);
    }

    inline virtual bool getValue() override {
        return _boolInput->getValue();
    }

    ProxyModuleUserInput * createProxy(const std::string &withName) override {
        return new ProxyModuleBoolUserInput(withName, _boolInput);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("ProxyModuleBoolUserInput does not support createPolyProxy");
    }

protected:
    BoolUserInput* _boolInput;
};

#endif //PATCHCORE_PROXYMODULEBOOLUSERINPUT_HPP
