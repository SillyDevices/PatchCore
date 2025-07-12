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

#ifndef PATCHCORE_POLYPROXYENUMUSERINPUT_HPP
#define PATCHCORE_POLYPROXYENUMUSERINPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/user/EnumUserInput.hpp"
#include "patchcore/module/input/user/ProxyModuleEnumUserInput.hpp"
#include "patchcore/module/input/PolyProxyUserInput.hpp"

class PolyProxyEnumUserInput : public PolyProxyUserInput, public EnumUserInput {
public:
    PolyProxyEnumUserInput(EnumUserInput *input, std::vector<Module *> modulesToProxy)
        : PolyProxyUserInput(), EnumUserInput(input->getName(), input->getValue()) {
        voiceProxyUserInputs.reserve(modulesToProxy.size());
        for (auto &module : modulesToProxy) {
            auto rawVoiceInput = module->getUserInput(input->getName());
            auto voiceInput = dynamic_cast<EnumUserInput *>(rawVoiceInput);
            if (voiceInput == nullptr) {
                throw std::runtime_error("PolyProxyEnumUserInput: Input is not a EnumUserInput");
            }
            voiceProxyUserInputs.push_back(nullptr);
            voiceInputs.push_back(voiceInput);
        }
    }
    virtual ~PolyProxyEnumUserInput() = default;
public:
    void setProxyVoiceInput(int voiceIndex, ProxyModuleUserInput *compositeInput) override{
        auto casted = dynamic_cast<ProxyModuleEnumUserInput *>(compositeInput);
        if (casted == nullptr) throw std::runtime_error("PolyProxyEnumUserInput: Composite input is not a CompositeModuleEnumUserInput");
        voiceProxyUserInputs[voiceIndex] = casted;
        proxyVoicesSet = true;
    }
public:
    EnumUserInput* getVoice(int voiceIndex) override {
        return voiceInputs[voiceIndex];
    }
    void setValue(int newValue) override {
        if (proxyVoicesSet) {
            for (auto &voiceInput : voiceProxyUserInputs) {
                voiceInput->setValue(newValue);
            }
        } else {
            for (auto &voiceInput : voiceInputs) {
                voiceInput->setValue(newValue);
            }
        }
    }

    ProxyModuleUserInput * createProxy(const std::string &withName) override {
        return new ProxyModuleEnumUserInput(withName, this);
    }

    std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override {
        throw std::runtime_error("PolyProxyEnumUserInput: createPolyProxy not implemented");
    }

protected:
    std::vector<EnumUserInput*> voiceInputs;
    std::vector<ProxyModuleEnumUserInput*> voiceProxyUserInputs;
};

#endif //PATCHCORE_POLYPROXYENUMUSERINPUT_HPP
