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
#include "patchcore/module/input/ExposedModuleUserInput.hpp"
#include "patchcore/module/output/PolyDemuxOutput.hpp"
#include "patchcore/module/output/PolyExposedOutput.hpp"
#include <algorithm>
#include <string>
#include <stdio.h>

#undef LOG_TAG
#define LOG_TAG "PolyModule"

PolyModule::PolyModule(ModuleFactory *factory, std::string name, int sampleRate, int voiceCount):
        PatchModule(factory, name, sampleRate) {
    this->voiceCount = voiceCount;
    polyProxies.reserve(voiceCount);
    _voices.reserve(voiceCount);
    voices.reserve(voiceCount);
    for (int i = 0; i < voiceCount; ++i) {
        auto module = std::make_unique<PatchModule>(factory, name, sampleRate);
        _voices.push_back(std::move(module));
        voices.push_back(_voices.back().get());
    }

    activeVoiceCount = voiceCount;
    setActiveVoiceCount(voiceCount);
}

PolyModule::~PolyModule() {
    for (auto* input : exposedInputs) {
        delete input;
    }
    exposedInputs.clear();

    for (auto* output : exposedOutputs) {
        delete output;
    }
    exposedOutputs.clear();

    for (auto* input : exposedUserInputs) {
        delete input;
    }
    exposedUserInputs.clear();

    for (auto* output : ownedProxyOutputs) {
        delete output;
    }
    ownedProxyOutputs.clear();
}

void PolyModule::onStartBuffer(int size) {
    Module::onStartBuffer(size);
    for (const auto &voice: voices) {
        voice->onStartBuffer(size);
    }
}

void PolyModule::processSample(int sampleIndex) {
    (void) sampleIndex;
    for (const auto &exposedInput : exposedInputs) {
        exposedInput->envelope();
    }
    for (const auto &proxyInput : proxyInputs) {
        proxyInput->envelope();
    }
    //todo optimize: only active voices
    for (int i = 0; i < activeVoiceCount; ++i) {
        voices[i]->processSample(sampleIndex);
    }
    for (const auto &proxyOutput : proxyOutputs) {
        proxyOutput->envelope();
    }
    for (const auto &exposedOutput : exposedOutputs) {
        exposedOutput->envelope();
    }
}

void PolyModule::processBlock() {
    for (const auto &exposedInput : exposedInputs) {
        auto* sourceBuffer = exposedInput->value.data();
        auto* targetBuffer = exposedInput->getModuleInput()->value.data();
        std::copy(sourceBuffer, sourceBuffer + PATCHCORE_BLOCK_SIZE, targetBuffer);
    }
    for (const auto &proxyInput : proxyInputs) {
        proxyInput->envelope();
    }
    //todo optimize: only active voices
    for (int i = 0; i < activeVoiceCount; ++i) {
        voices[i]->processBlock();
    }
    for (const auto &proxyOutput : proxyOutputs) {
        proxyOutput->envelope();
    }
    for (const auto &exposedOutput : exposedOutputs) {
        auto* sourceBuffer = exposedOutput->getModuleOutput()->value.data();
        auto* targetBuffer = exposedOutput->value.data();
        std::copy(sourceBuffer, sourceBuffer + PATCHCORE_BLOCK_SIZE, targetBuffer);
    }
}

ExposedModuleInput *PolyModule::exposeInput(ModuleInput *input, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyInput *>(input);
    if (casted == nullptr) throw std::runtime_error("PolyModule::exposeInput: input is not a PolyProxyInput");
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceInput = voices[i]->exposeInput(casted->getVoice(i), withName);
        casted->setCompositeVoiceInput(i, compositeVoiceInput);
        casted->setVoiceInput(i, compositeVoiceInput);
    }
    auto* exposedInput = casted->createExposed(withName);
    exposedInputs.push_back(exposedInput);
    registerInput(*exposedInput, withName);
    proxyInputs.push_back(casted);
    return exposedInput;
}

ExposedModuleOutput *PolyModule::exposeOutput(ModuleOutput *output, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyOutput *>(output);
    if (casted == nullptr) throw std::runtime_error("PolyModule::exposeOutput: output is not a PolyProxyOutput");
    auto* exposedOutput = new PolyExposedOutput(casted, withName);
    // Expose each voice output so nested voice graphs keep their output path reachable.
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceOutput = voices[i]->exposeOutput(casted->getVoice(i), withName);
        exposedOutput->setVoiceOutput(i, compositeVoiceOutput);
    }
    registerOutput(*exposedOutput, withName);
    ownedProxyOutputs.push_back(exposedOutput);
    proxyOutputs.push_back(exposedOutput);
    return exposedOutput;
}

ExposedModuleOutput *PolyModule::exposeDemuxOutput(ModuleOutput *output, const std::string &withName, const int defaultVoiceIndex) {
    if (output == nullptr) throw std::invalid_argument("Output cannot be null");
    auto casted = dynamic_cast<PolyProxyOutput *>(output);
    if (casted == nullptr) throw std::runtime_error("PolyModule::exposeDemuxOutput: output is not a PolyProxyOutput");
    auto* demuxOutput = new PolyDemuxOutput(casted, withName, defaultVoiceIndex);
    // pass corresponding module output to each voice
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceOutput = voices[i]->exposeOutput(casted->getVoice(i), withName);
        demuxOutput->setVoiceOutput(i, compositeVoiceOutput);
    }
    registerOutput(*demuxOutput, withName);
    ownedProxyOutputs.push_back(demuxOutput);
    proxyOutputs.push_back(demuxOutput);
    return demuxOutput;
}

UserInput *PolyModule::exposeUserInput(UserInput *input, const std::string &withName) {
    auto casted = dynamic_cast<PolyProxyUserInput *>(input);
    if (casted == nullptr) throw std::runtime_error("PolyModule::exposeUserInput: input is not a PolyProxyUserInput");
    // pass corresponding module output to each voice
    for (int i = 0; i < voiceCount; ++i) {
        auto compositeVoiceUserInput = voices[i]->exposeUserInput(casted->getVoice(i), withName);
        casted->setProxyVoiceInput(i, dynamic_cast<ExposedModuleUserInput *>(compositeVoiceUserInput));
        casted->setVoiceInput(i, dynamic_cast<ExposedModuleUserInput *>(compositeVoiceUserInput));
    }
    auto* polyUserInput = dynamic_cast<UserInput *>(casted);
    if (polyUserInput == nullptr) throw std::runtime_error("PolyModule::exposeUserInput: casted input is not a UserInput");
    auto* exposedUserInput = polyUserInput->createExposed(withName);
    auto castedExposedUserInput = dynamic_cast<UserInput *>(exposedUserInput);
    if (castedExposedUserInput == nullptr) throw std::runtime_error("PolyModule::exposeUserInput: exposed input is not a UserInput");
    exposedUserInputs.push_back(exposedUserInput);
    registerUserInput(*castedExposedUserInput, withName);
    return castedExposedUserInput;
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

void PolyModule::resetPatch() {
    for (auto &voice : voices) {
        voice->resetPatch();
    }
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

void PolyModule::removePatch(ModuleOutput* from, ModuleInput* to) {
    auto castedOutput = dynamic_cast<PolyProxyOutput *>(from);
    if (castedOutput == nullptr) throw std::runtime_error("PolyModule::addPatch: output is not a PolyProxyOutput");
    auto castedInput = dynamic_cast<PolyProxyInput *>(to);
    if (castedInput == nullptr) throw std::runtime_error("PolyModule::addPatch: input is not a PolyProxyInput");
    for (int i = 0; i < voiceCount; ++i) {
        auto voiceOutput = castedOutput->getVoice(i);
        auto voiceInput = castedInput->getVoice(i);
        if (voiceOutput == nullptr || voiceInput == nullptr) {
            throw std::runtime_error("PolyModule::addPatch: Voice output or input is null");
        }
        voices[i]->removePatch(voiceOutput, voiceInput);
    }
}

size_t PolyModule::getVoiceCount() const {
    return voiceCount;
}

PatchModule *PolyModule::getVoice(size_t index) {
    if (index >= voiceCount) {
        throw std::out_of_range("Voice index out of range");
    }
    return voices[index];
}


size_t PolyModule::getActiveVoiceCount() const{
    return activeVoiceCount;
}

void PolyModule::setActiveVoiceCount(size_t count) {
    if (count > voiceCount) {
        throw std::out_of_range("Active voice count cannot be greater than total voice count");
    }
    activeVoiceCount = count;
}
