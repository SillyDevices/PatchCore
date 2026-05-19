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

#include "patchcore/module/PolyProxyPatchModule.hpp"
#include "patchcore/module/PolyModule.hpp"
#include <stdexcept>

PolyProxyPatchModule::PolyProxyPatchModule(ModuleFactory *factory, const PatchModule *module, PolyModule *polyModule)
        :
        PolyProxyModule(module),
        PolyModule(factory, module->getModuleName(), module->getSampleRate(), polyModule->getVoiceCount())

        {
    this->voices.clear();
    for (int i =0; i < polyModule->getVoiceCount(); ++i) {
        auto voice = polyModule->getVoice(i);
        auto self = dynamic_cast<PatchModule*>(voice->getModule(module->getModuleName()));
        if (self == nullptr) throw std::runtime_error("PolyProxyPatchModule::PolyProxyPatchModule: could not find voice module");
        this->voices.push_back(self);
        this->modulesToProxy.push_back(self);
    }

    auto inputs = module->getModuleInputs();
    for (const auto& input : inputs) {
        polyProxyInputs.push_back(std::make_unique<PolyProxyInput>(input.second, modulesToProxy));
        auto polyProxyInput = polyProxyInputs.back().get();
        this->PolyModule::registerInput(*polyProxyInput);
    }

    auto outputs = module->getModuleOutputs();
    for (const auto& output : outputs) {
        polyProxyOutputs.push_back(std::make_unique<PolyProxyOutput>(output.second, modulesToProxy));
        auto polyProxyOutput = polyProxyOutputs.back().get();
        this->PolyModule::registerOutput(*polyProxyOutput);
    }

    auto userInputs = module->getUserInputs();
    for (const auto& userInput : userInputs) {
        polyProxyUserInputs.push_back(userInput.second->createPolyProxy(modulesToProxy));
        auto polyProxyUserInput = polyProxyUserInputs.back().get();
        auto input = dynamic_cast<UserInput *>(polyProxyUserInput);
        if (input == nullptr) {
            throw std::runtime_error("PolyProxyPatchModule::PolyProxyPatchModule: UserInput is not of type UserInput");
        }
        this->PolyModule::registerUserInput(*input);
    }
}


const std::unordered_map<std::string, ModuleInput *>& PolyProxyPatchModule::getModuleInputs() const {
    return PolyModule::getModuleInputs();
}

ModuleInput* PolyProxyPatchModule::getModuleInput(const std::string& moduleName) {
    return PolyModule::getModuleInput(moduleName);
}

const std::unordered_map<std::string, ModuleOutput *>& PolyProxyPatchModule::getModuleOutputs() const {
    return PolyModule::getModuleOutputs();
}

ModuleOutput * PolyProxyPatchModule::getModuleOutput(const std::string& outputName) {
    return PolyModule::getModuleOutput(outputName);
}

const std::unordered_map<std::string, UserInput *>& PolyProxyPatchModule::getUserInputs() const {
    return PolyModule::getUserInputs();
}

UserInput *PolyProxyPatchModule::getUserInput(const std::string& inputName) {
    return PolyModule::getUserInput(inputName);
}





