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

#ifndef PATCHCORE_POLYPROXYBOOLUSERINPUT_HPP
#define PATCHCORE_POLYPROXYBOOLUSERINPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"
#include "patchcore/module/input/user/ProxyModuleBoolUserInput.hpp"
#include "patchcore/module/input/PolyProxyUserInput.hpp"

class PolyProxyBoolUserInput : public PolyProxyUserInput, public BoolUserInput {
public:
    PolyProxyBoolUserInput(BoolUserInput *input, std::vector<Module *> modulesToProxy)
        : PolyProxyUserInput(), BoolUserInput(input->getName()) {
        voiceProxyUserInputs.reserve(modulesToProxy.size());
        for (auto &module : modulesToProxy) {
            auto rawVoiceInput = module->getUserInput(input->getName());
            auto voiceInput = dynamic_cast<BoolUserInput *>(rawVoiceInput);
            if (voiceInput == nullptr) {
                throw std::runtime_error("PolyProxyBoolUserInput: Input is not a BoolUserInput");
            }
            voiceProxyUserInputs.push_back(nullptr);
            if (voiceInput) {
                voiceInputs.push_back(voiceInput);
            } else {
                throw std::runtime_error("PolyProxyBoolUserInput: Input not found in module");
            }
        }
    }
    virtual ~PolyProxyBoolUserInput() = default;
public:
    void setProxyVoiceInput(int voiceIndex, ProxyModuleUserInput *compositeInput) override {
        auto casted = dynamic_cast<ProxyModuleBoolUserInput *>(compositeInput);
        if (casted == nullptr) throw std::runtime_error("PolyProxyBoolUserInput: Composite input is not a CompositeModuleBoolUserInput");
        voiceProxyUserInputs[voiceIndex] = casted;
        proxyVoicesSet = true;
    }
public:
    BoolUserInput* getVoice(int voiceIndex) override {
        return voiceInputs[voiceIndex];
    }
    void setValue(bool newValue) override {
        if (proxyVoicesSet) {
            for (auto &voiceInput: voiceProxyUserInputs) {
                voiceInput->setValue(newValue);
            }
        } else {
            for (auto &voiceInput: voiceInputs) {
                voiceInput->setValue(newValue);
            }
        }
    }

    ProxyModuleUserInput * createProxy(const std::string &withName) override {
        return new ProxyModuleBoolUserInput(withName, this);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("PolyProxyBoolUserInput: createPolyProxy not implemented");
    }
protected:
    std::vector<BoolUserInput*> voiceInputs;
    std::vector<ProxyModuleBoolUserInput*> voiceProxyUserInputs;
};

#endif //PATCHCORE_POLYPROXYBOOLUSERINPUT_HPP
