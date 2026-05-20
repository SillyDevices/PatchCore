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

#ifndef PATCHCORE_POLYPROXYFLOATUSERINPUT_HPP
#define PATCHCORE_POLYPROXYFLOATUSERINPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/PolyProxyUserInput.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/ExposedModuleFloatUserInput.hpp"

class PolyProxyFloatUserInput : public FloatUserInput, public PolyProxyUserInput {
public:
    PolyProxyFloatUserInput(FloatUserInput *input, std::vector<Module *> modulesToProxy)
    : FloatUserInput(input->getName(), input->getSpeed()), PolyProxyUserInput() {
        voiceExposedModuleUserInputs.reserve(modulesToProxy.size());
        for (auto &module : modulesToProxy) {
            auto rawVoiceInput = module->getUserInput(input->getName());
            auto voiceInput = dynamic_cast<FloatUserInput *>(rawVoiceInput);
            if (voiceInput == nullptr) {
                throw std::runtime_error("PolyProxyFloatUserInput: Input is not a FloatUserInput");
            }
            voiceExposedModuleUserInputs.push_back(nullptr);
            voiceInputs.push_back(voiceInput);
        }
    }
    virtual ~PolyProxyFloatUserInput() = default;
public:
    void setVoiceInput(int voiceIndex, ExposedModuleUserInput *voiceInput) override {
        auto casted = dynamic_cast<ExposedModuleFloatUserInput *>(voiceInput);
        if (casted == nullptr) throw std::runtime_error("PolyProxyFloatUserInput: Composite input is not a CompositeModuleFloatUserInput");
        voiceInputs[voiceIndex] = casted;
    }

    void setProxyVoiceInput(int voiceIndex, ExposedModuleUserInput *compositeInput) override {
        auto casted = dynamic_cast<ExposedModuleFloatUserInput *>(compositeInput);
        if (casted == nullptr) throw std::runtime_error("PolyProxyFloatUserInput: Composite input is not a CompositeModuleFloatUserInput");
        voiceExposedModuleUserInputs[voiceIndex] = casted;
        proxyVoicesSet = true;
    }
public:
    FloatUserInput* getVoice(int voiceIndex) override {
        return voiceInputs[voiceIndex];
    }
    void setValue(float newValue) override {
        if (proxyVoicesSet) {
            for (auto &voiceInput: voiceExposedModuleUserInputs) {
                voiceInput->setValue(newValue);
            }
        } else {
            for (auto &voiceInput: voiceInputs) {
                voiceInput->setValue(newValue);
            }
        }
    }
    void setParameterLockValue(float newValue) override {
        if (proxyVoicesSet) {
            for (auto &voiceInput : voiceExposedModuleUserInputs) {
                voiceInput->setParameterLockValue(newValue);
            }
        } else {
            for (auto &voiceInput : voiceInputs) {
                voiceInput->setParameterLockValue(newValue);
            }
        }
    }
    void clearParameterLock() override {
        if (proxyVoicesSet) {
            for (auto &voiceInput: voiceExposedModuleUserInputs) {
                voiceInput->clearParameterLock();
            }
        } else {
            for (auto &voiceInput: voiceInputs) {
                voiceInput->clearParameterLock();
            }
        }
    }
    void prepareBlock(const BlockContext& context) override {
        (void) context;
        // DO NOTHING
        // prepareBlock on voice inputs is handled by voice modules directly
    }

    ExposedModuleUserInput * createExposed(const std::string &withName) override {
        return new ExposedModuleFloatUserInput(withName, this);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("PolyProxyFloatUserInput cannot create another PolyProxyFloatUserInput");
    }
protected:
    std::vector<FloatUserInput*> voiceInputs;
    std::vector<ExposedModuleFloatUserInput*> voiceExposedModuleUserInputs;
};

#endif //PATCHCORE_POLYPROXYFLOATUSERINPUT_HPP
