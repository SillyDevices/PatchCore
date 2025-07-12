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

#ifndef DIGITRON_PROXYMODULEFLOATUSERINPUT_HPP
#define DIGITRON_PROXYMODULEFLOATUSERINPUT_HPP

#include "patchcore/module/input/ProxyModuleUserInput.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

class ProxyModuleFloatUserInput: public FloatUserInput, public ProxyModuleUserInput {
public:
    ProxyModuleFloatUserInput(std::string name, FloatUserInput *userInput)
        : FloatUserInput(name), ProxyModuleUserInput(userInput), _floatInput(userInput){
    };

    virtual inline void setValue(float newValue) override {
        _floatInput->setValue(newValue);
    }

    inline void setParameterLockValue(float newValue) override {
        _floatInput->setParameterLockValue(newValue);
    }

    void clearParameterLock() override {
        _floatInput->clearParameterLock();
    }

    void onStartBuffer(int bufferSize) override {
        _floatInput->onStartBuffer(bufferSize);
    }

    virtual void envelope() override {
        _floatInput->envelope();
    }

    ProxyModuleUserInput * createProxy(const std::string &withName) override {
        return new ProxyModuleFloatUserInput(withName, this);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("ProxyModuleFloatUserInput does not support createPolyProxy");
    }

protected:
    FloatUserInput *_floatInput;
};

#endif //DIGITRON_PROXYMODULEFLOATUSERINPUT_HPP
