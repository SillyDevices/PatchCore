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

#ifndef PATCHCORE_USERINPUT_HPP
#define PATCHCORE_USERINPUT_HPP

#include "patchcore/module/input/ModuleInput.hpp"


class Module;
class ProxyModuleUserInput;
class PolyProxyUserInput;

enum class UserInputType {
    FLOAT = 0,
    ENUM,
    BOOL
};

class UserInput {
public:
    UserInput(std::string name, UserInputType type): name(name), type(type) {};
    virtual ~UserInput() = default;

    inline std::string getName() const {
        return name;
    };

public:
    void setModule(Module* module) {
        _module = module;
    };

    Module* getModule() {
        return _module;
    };


public:
    virtual ProxyModuleUserInput* createProxy(const std::string &withName) = 0;
    virtual std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) = 0;

private:
    Module* _module;

public:
    std::string name;
    UserInputType type;


};

#endif //PATCHCORE_USERINPUT_HPP
