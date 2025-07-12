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

#include "patchcore/module/PolyProxyModule.hpp"
#include "patchcore/module/PolyModule.hpp"

PolyProxyModule::PolyProxyModule(const Module* module, PolyModule* polyModule)
    : Module(module->getModuleName(), module->getSampleRate()) {

    auto voices = polyModule->getVoiceCount();
    modulesToProxy = std::vector<Module *>();
    for (int i = 0; i < voices; ++i) {
        auto voiceModule = polyModule->getVoice(i);
        if (voiceModule) {
            modulesToProxy.push_back(voiceModule->getModule(module->getModuleName()));
        } else {
            throw std::runtime_error("PolyProxyModule: Voice module not found");
        }
    }

    auto inputs = module->getModuleInputs();
    for (const auto& input : inputs) {
        polyProxyInputs.push_back(std::make_unique<PolyProxyInput>(input.second, modulesToProxy));
        auto polyProxyInput = polyProxyInputs.back().get();
        registerInput(*polyProxyInput);
    }

    auto outputs = module->getModuleOutputs();
    for (const auto& output : outputs) {
        polyProxyOutputs.push_back(std::make_unique<PolyProxyOutput>(output.second, modulesToProxy));
        auto polyProxyOutput = polyProxyOutputs.back().get();
        registerOutput(*polyProxyOutput);
    }
    auto userInputs = module->getUserInputs();
    for (const auto& userInput : userInputs) {
        polyProxyUserInputs.push_back(userInput.second->createPolyProxy(modulesToProxy));
        auto polyProxyUserInput = polyProxyUserInputs.back().get();
        auto input = dynamic_cast<UserInput *>(polyProxyUserInput);
        if (input == nullptr) throw std::runtime_error("PolyProxyModule: UserInput is not of type UserInput");
        registerUserInput(*input);
    }

}

