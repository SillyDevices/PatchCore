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

#ifndef PATCHCORE_POLYPROXYOUTPUT_HPP
#define PATCHCORE_POLYPROXYOUTPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"

class PolyProxyOutput : public ModuleOutput {
public:
    PolyProxyOutput(ModuleOutput *output, std::vector<Module *> modulesToProxy)
        : ModuleOutput(output->getName()) {
        for (auto &module : modulesToProxy) {
            auto voiceOutput = module->getModuleOutput(output->getName());
            if (voiceOutput) {
                voiceOutputs.push_back(voiceOutput);
            } else {
                throw std::runtime_error("PolyProxyOutput: Output not found in module");
            }
        }
    }
    virtual ~PolyProxyOutput() = default;
public:
    ModuleOutput* getVoice(int voiceIndex) {
        return voiceOutputs[voiceIndex];
    }
    void envelope() {
        value = 0.0f;
        for (auto &voiceOutput : voiceOutputs) {
            value += voiceOutput->value;
        }
    }
protected:
    std::vector<ModuleOutput*> voiceOutputs;
};

#endif //PATCHCORE_POLYPROXYOUTPUT_HPP
