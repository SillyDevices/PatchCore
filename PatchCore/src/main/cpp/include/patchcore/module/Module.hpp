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

#ifndef PATCHCORE_MODULE_HPP
#define PATCHCORE_MODULE_HPP

#include "patchcore/module/input/ModuleInput.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"
#include "patchcore/module/input/UserInput.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"
#include "patchcore/module/input/user/EnumUserInput.hpp"

#include <unordered_map>
#include <string>
#include <vector>

class PolyModule;
class PolyProxyModule;

class Module: public OnStartBuffer {
public:
    Module(std::string name, int sampleRate);
    Module(const Module& other) = delete;
    virtual ~Module() = default;

    virtual void envelope() = 0;

    const std::string& getModuleName() const;

    virtual const std::unordered_map<std::string, ModuleInput *>& getModuleInputs() const;
    virtual ModuleInput* getModuleInput(const std::string& moduleName);

    virtual const std::unordered_map<std::string, ModuleOutput *>& getModuleOutputs() const;
    virtual ModuleOutput* getModuleOutput(const std::string& outputName);

    virtual const std::unordered_map<std::string, UserInput *>& getUserInputs() const;
    virtual UserInput* getUserInput(const std::string& inputName);
    virtual const std::vector<FloatUserInput *>& getInterpolatedInputs();

    virtual std::unique_ptr<Module> clone() const = 0;
    //TODO rename to createPolyProxyModule
    virtual std::unique_ptr<PolyProxyModule> createPolyModuleProxy(PolyModule* polyModule) const;

    void onStartBuffer(int size) override;

    int getSampleRate() const;

protected:

    virtual void registerInput(ModuleInput& input, const std::string& withName = "") final {
        if (!withName.empty()) {
            inputs[withName] = &input;
        } else {
            inputs[input.getName()] = &input;
        }
        input.setModule(this);
    }

    virtual void registerOutput(ModuleOutput& output, const std::string& withName = "") final {
        if (!withName.empty()) {
            outputs[withName] = &output;
        } else {
            outputs[output.getName()] = &output;
        }
        output.setModule(this);
    }

    virtual void registerUserInput(UserInput& input, const std::string& withName = "") final {
        if (!withName.empty()) {
            userInputs[withName] = &input;
        } else {
            userInputs[input.getName()] = &input;
        }
        if (auto floatInput = dynamic_cast<FloatUserInput *>(&input)) {
            printf("Registering FloatUserInput: %s\n", floatInput->getName().c_str());
            interpolatedInputs.push_back(floatInput);
        } else {
            printf("Registering UserInput: %s\n", input.getName().c_str());
        }
        input.setModule(this);
    }

    void copyIOs(const Module& other) {
        for (auto &input: other.inputs) {
            getModuleInput(input.second->getName())->value = input.second->value;
        }
        for (auto &output: other.outputs) {
            getModuleOutput(output.second->getName())->value = output.second->value;
        }
        for (auto &userInput: other.userInputs) {
            auto userInputPtr = getUserInput(userInput.second->getName());
            auto type = userInputPtr->type;
            if (type == UserInputType::BOOL) {
                auto* boolInput = dynamic_cast<BoolUserInput*>(userInputPtr);
                auto* otherBoolInput = dynamic_cast<BoolUserInput*>(userInput.second);
                if (!boolInput) throw std::invalid_argument("Input is not BoolUserInput");
                boolInput->copyFrom(*otherBoolInput);
            } else if (type == UserInputType::ENUM) {
                auto* enumInput = dynamic_cast<EnumUserInput*>(userInputPtr);
                auto* otherEnumInput = dynamic_cast<EnumUserInput*>(userInput.second);
                if (!enumInput) throw std::invalid_argument("Input is not EnumUserInput");
                enumInput->copyFrom(*otherEnumInput);
            } else if (type == UserInputType::FLOAT) {
                auto* floatInput = dynamic_cast<FloatUserInput*>(userInputPtr);
                auto* otherFloatInput = dynamic_cast<FloatUserInput*>(userInput.second);
                if (!floatInput) throw std::invalid_argument("Input is not FloatUserInput");
                floatInput->copyFrom(*otherFloatInput);
            } else {
                throw std::invalid_argument("Invalid UserInput type");
            }
        }
    }

public:
    [[nodiscard]]
    virtual bool needEnvelopeOnInputConnection() const {
        return false;
    }
protected:
    std::unordered_map<std::string, ModuleInput *> inputs = {};
    std::unordered_map<std::string, ModuleOutput *> outputs = {};
    std::unordered_map<std::string, UserInput *> userInputs = {};
    std::vector<FloatUserInput *> interpolatedInputs = {};

    int sampleRate;
    std::string name;
};

#endif //PATCHCORE_MODULE_HPP
