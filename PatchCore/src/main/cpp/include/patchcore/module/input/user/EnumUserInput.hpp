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

#ifndef PATCHCORE_ENUMUSERINPUT_HPP
#define PATCHCORE_ENUMUSERINPUT_HPP

#include "patchcore/module/input/UserInput.hpp"
#include <string>

class EnumUserInput: public UserInput {
public:
    explicit EnumUserInput(std::string name, int defaultValue = 0): UserInput(name, UserInputType::ENUM) {
        currentWaveForm = defaultValue;
    };
    virtual ~EnumUserInput() = default;
public:

    virtual inline void setValue(int value){
     currentWaveForm = value;
    }

    virtual inline int getValue(){
        return currentWaveForm;
    }

public:
    void copyFrom(UserInput &other) {
        if (other.type != UserInputType::ENUM) {
            throw std::runtime_error("Cannot copy from non-bool UserInput");
        }
        currentWaveForm = static_cast<EnumUserInput&>(other).currentWaveForm;
    }

    virtual ProxyModuleUserInput * createProxy(const std::string &withName) override;
    virtual std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override;

private:
    int currentWaveForm = 0;
};

#endif //PATCHCORE_ENUMUSERINPUT_HPP
