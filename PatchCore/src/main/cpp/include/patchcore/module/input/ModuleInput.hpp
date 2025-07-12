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

#ifndef PATCHCORE_MODULEINPUT_HPP
#define PATCHCORE_MODULEINPUT_HPP

#include "patchcore/module/input/Input.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"
#include <string>
#include <vector>

class Module;
class ProxyModuleInput;

//TODO move implementation to .cpp
class ModuleInput: Input {
public:
    //disconnected value used only in Modulation and Wavefolder
    explicit ModuleInput(std::string inputName, float disconnectedValue = 0.0f);
    virtual ~ModuleInput() = default;

    inline std::string getName() const {
        return name;
    };


    //TODO remove
    inline void disconnect() {
        value = disconnectedValue;
        _isConnected = false;
        outputs.clear();
    };

    //TODO remove
    inline void disconnectAll() {
        value = disconnectedValue;
        _isConnected = false;
        outputs.clear();
    };

    //TODO remove
    inline void connect(ModuleOutput* output) {
        _isConnected = true;
        outputs.push_back(output);
    };

    //TODO remove
    inline bool isConnected() {
        return _isConnected;
    };

    //TODO remove
    void envelope() {
        printf("ModuleInput::envelope() called for input: %s\n", name.c_str());
        if(_isConnected) {
            auto computedValue = 0.0f;
            for (auto output : outputs) {
                computedValue += output->value;
            }
            this->value = computedValue;
        }
    };

//    virtual inline void setValue(float newValue) override {
//        value = newValue;
//    };
//
//    virtual inline float getValue() {
//        return value;
//    };

    virtual ProxyModuleInput *createProxy(const std::string &withName);

public:

    void setModule(Module* module) {
        _module = module;
    };

    Module* getModule() const {
        return _module;
    };

    float getDisconnectedValue() const {
        return disconnectedValue;
    };

protected:
    std::string name;
    float disconnectedValue;

private:
    Module* _module;

public:
    float value = 0.0f;
    bool _isConnected = false;
    std::vector<ModuleOutput*> outputs = {};

};

#endif //PATCHCORE_MODULEINPUT_HPP
