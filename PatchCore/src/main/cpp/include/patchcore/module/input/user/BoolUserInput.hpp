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

#ifndef PATCHCORE_BOOLUSERINPUT_HPP
#define PATCHCORE_BOOLUSERINPUT_HPP

#include "patchcore/module/input/UserInput.hpp"
#include <string>

class BoolUserInput: public UserInput {
public:
    explicit BoolUserInput(std::string name, bool initial = false): UserInput(name, UserInputType::BOOL), currentValue(initial) {};
    virtual ~BoolUserInput() = default;
public:

    inline virtual void setValue(bool value){
        currentValue = value;
    }

    inline virtual bool getValue(){
        return currentValue;
    }

public:
    void copyFrom(UserInput &other) {
        if (other.type != UserInputType::BOOL) {
            throw std::runtime_error("Cannot copy from non-bool UserInput");
        }
        currentValue = dynamic_cast<BoolUserInput&>(other).currentValue;
    }

    ProxyModuleUserInput * createProxy(const std::string &withName) override;
    virtual std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override;

private:
    bool currentValue;
};

#endif //PATCHCORE_BOOLUSERINPUT_HPP
