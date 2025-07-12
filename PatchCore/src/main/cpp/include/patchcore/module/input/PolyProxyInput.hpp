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

#ifndef PATCHCORE_POLYPROXYINPUT_HPP
#define PATCHCORE_POLYPROXYINPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/ModuleInput.hpp"
#include "patchcore/module/input/ProxyModuleInput.hpp"

class PolyProxyInput : public ModuleInput {
public:
    PolyProxyInput(ModuleInput *input, std::vector<Module *> modulesToProxy)
        : ModuleInput(input->getName(), input->getDisconnectedValue()) {
        voiceProxyInputs.reserve(modulesToProxy.size());
        for (auto &module : modulesToProxy) {
            auto voiceInput = module->getModuleInput(input->getName());
            voiceProxyInputs.push_back(nullptr);
            if (voiceInput) {
                voiceInputs.push_back(voiceInput);
            } else {
                throw std::runtime_error("PolyProxyInput: Input not found in module");
            }
        }
    }
    virtual ~PolyProxyInput() = default;
public:
    void setCompositeVoiceInput(int voiceIndex, ProxyModuleInput *compositeInput) {
        voiceProxyInputs[voiceIndex] = compositeInput;
    }
public:
    ModuleInput* getVoice(int voiceIndex) {
        return voiceInputs[voiceIndex];
    }
    void envelope() {
        for (auto &voiceInput : voiceProxyInputs) {
            voiceInput->value = value;
        }
    }
protected:
    std::vector<ModuleInput*> voiceInputs;
    std::vector<ProxyModuleInput*> voiceProxyInputs;
};

#endif //PATCHCORE_POLYPROXYINPUT_HPP
