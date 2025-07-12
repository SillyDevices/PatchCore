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

#include "patchcore/module/PolyModule.hpp"
#include "patchcore/module/PolyProxyModule.hpp"
#include <string>
#include <stdio.h>

PolyModule::PolyModule(ModuleFactory *factory, std::string name, int sampleRate, int voiceCount):
        PatchModule(factory, name, sampleRate) {
    this->voiceCount = voiceCount;
    polyProxies.reserve(voiceCount);
    voices.reserve(voiceCount);
    for (int i = 0; i < voiceCount; ++i) {
        auto module = std::make_unique<PatchModule>(factory, name, sampleRate);
        voices.push_back(std::move(module));
    }
}

void PolyModule::onStartBuffer(int size) {
    for (const auto &voice: voices) {
        voice->onStartBuffer(size);
    }
}

void PolyModule::envelope() {
    for (const auto &proxyInput : proxyInputs) {
        proxyInput->envelope();
    }
    for (const auto &voice: voices) {
        voice->envelope();
    }
    for (const auto &proxyOutput : proxyOutputs) {
        proxyOutput->envelope();
    }
}

ProxyModuleInput *PolyModule::addInput(ModuleInput *input, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyInput *>(input);
    if (casted == nullptr) throw std::runtime_error("PolyModule::addInput: input is not a PolyProxyInput");
    // pass corresponding module input to each voice
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceInput = voices[i]->addInput(casted->getVoice(i), withName);
        casted->setCompositeVoiceInput(i, compositeVoiceInput);
    }
    registerInput(*casted, withName);
    proxyInputs.push_back(casted);
    return nullptr;
}

ProxyModuleOutput *PolyModule::addOutput(ModuleOutput *output, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyOutput *>(output);
    if (casted == nullptr) throw std::runtime_error("PolyModule::addOutput: output is not a PolyProxyOutput");
    // pass corresponding module output to each voice
    for (int i = 0; i < voiceCount; ++i) {
        voices[i]->addOutput(casted->getVoice(i), withName);
    }
    registerOutput(*casted, withName);
    proxyOutputs.push_back(casted);
    return nullptr;
}

UserInput *PolyModule::addUserInput(UserInput *input, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyUserInput *>(input);
    if (casted == nullptr) throw std::runtime_error("PolyModule::addUserInput: input is not a PolyProxyUserInput");
    // pass corresponding module output to each voice
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceUserInput = voices[i]->addUserInput(casted->getVoice(i), withName);
        casted->setProxyVoiceInput(i, dynamic_cast<ProxyModuleUserInput *>(compositeVoiceUserInput));
    }
    auto userInput = dynamic_cast<UserInput *>(casted);
    if (userInput == nullptr) throw std::runtime_error("PolyModule::addUserInput: casted input is not a UserInput");
    registerUserInput(*userInput, withName);
    proxyUserInputs.push_back(casted);
    return nullptr;
}

Module *PolyModule::createModule(const std::string &moduleTypeName, const std::string &moduleName,
                                 const std::map<std::string, ModuleParameter> &parameters) {

    Module* module = nullptr;
    for (auto &voice : voices) {
        module = voice->createModule(moduleTypeName, moduleName, parameters);
    }
    auto proxy = module->createPolyModuleProxy(this);
    polyProxies.push_back(std::move(proxy));
    return polyProxies.back().get();
}

Module *PolyModule::addModule(std::unique_ptr<Module> module) {
    for (auto &voice : voices) {
        auto clone = module->clone();
        voice->addModule(std::move(clone));
    }
    auto proxy = module->createPolyModuleProxy(this);
    polyProxies.push_back(std::move(proxy));
    return polyProxies.back().get();
}

void PolyModule::clearModules() {
    for (auto &voice : voices) {
        voice->clearModules();
    }
}

Module *PolyModule::getModule(const std::string &moduleName) const {
    for (auto &proxy : polyProxies) {
        if (proxy->getModuleName() == moduleName) {
            return proxy.get();
        }
    }
    throw std::runtime_error("Module " + moduleName + " not found in PolyModule");
}

void PolyModule::addPatch(ModuleOutput *output, ModuleInput *input) {
    auto castedOutput = dynamic_cast<PolyProxyOutput *>(output);
    if (castedOutput == nullptr) throw std::runtime_error("PolyModule::addPatch: output is not a PolyProxyOutput");
    auto castedInput = dynamic_cast<PolyProxyInput *>(input);
    if (castedInput == nullptr) throw std::runtime_error("PolyModule::addPatch: input is not a PolyProxyInput");
    for (int i = 0; i < voiceCount; ++i) {
        auto voiceOutput = castedOutput->getVoice(i);
        auto voiceInput = castedInput->getVoice(i);
        if (voiceOutput == nullptr || voiceInput == nullptr) {
            throw std::runtime_error("PolyModule::addPatch: Voice output or input is null");
        }
        voices[i]->addPatch(voiceOutput, voiceInput);
    }
}

void PolyModule::resetPatch() {
    for (auto &voice : voices) {
        voice->resetPatch();
    }
}

size_t PolyModule::getVoiceCount() const {
    return voiceCount;
}

PatchModule *PolyModule::getVoice(size_t index) {
    if (index >= voices.size()) {
        throw std::out_of_range("Voice index out of range");
    }
    return voices[index].get();
}










