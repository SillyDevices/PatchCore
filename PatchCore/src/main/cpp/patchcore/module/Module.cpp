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

#include "patchcore/module/Module.hpp"
#include "patchcore/module/PolyModule.hpp"
#include "patchcore/module/PolyProxyModule.hpp"

Module::Module(std::string name, int sampleRate) : sampleRate(sampleRate), name(name) {};


const std::string& Module::getModuleName() const {
    return name;
}


const std::unordered_map<std::string, ModuleInput *>& Module::getModuleInputs() const {
    return inputs;
}

ModuleInput *Module::getModuleInput(const std::string& moduleName) {
    return inputs[moduleName];
}


const std::unordered_map<std::string, ModuleOutput *>& Module::getModuleOutputs() const {
    return outputs;
}

ModuleOutput *Module::getModuleOutput(const std::string& outputName) {
    return outputs[outputName];
}


const std::unordered_map<std::string, UserInput *>& Module::getUserInputs() const {
    return userInputs;
}

UserInput *Module::getUserInput(const std::string& inputName) {
    auto input = userInputs[inputName];
    return input;
}

void Module::onStartBuffer(int size) {
    for (auto &input : interpolatedInputs) {
        input->onStartBuffer(size);
    }
}

int Module::getSampleRate() const {
    return sampleRate;
}

const std::vector<FloatUserInput *> &Module::getInterpolatedInputs() {
    return interpolatedInputs;
}

std::unique_ptr<PolyProxyModule> Module::createPolyModuleProxy(PolyModule* polyModule) const {
    return std::make_unique<PolyProxyModule>(this, polyModule);
}