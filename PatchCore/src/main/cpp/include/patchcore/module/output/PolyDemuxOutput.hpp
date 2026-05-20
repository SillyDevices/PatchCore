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

#ifndef PATCHCORE_POLYDEMUXOUTPUT_HPP
#define PATCHCORE_POLYDEMUXOUTPUT_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/output/ExposedModuleOutput.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"
#include "patchcore/module/output/PolyProxyOutput.hpp"

class PolyDemuxOutput : public PolyProxyOutput, public ExposedModuleOutput {
public:
    PolyDemuxOutput(PolyProxyOutput *output, const std::string& withName, int defaultVoiceIndex)
        : ModuleOutput(withName), PolyProxyOutput(*output), ExposedModuleOutput(withName, output), _voiceIndex(defaultVoiceIndex) { }
    ~PolyDemuxOutput() override = default;
public:
    void selectVoiceBlock() {
        value = getVoice(_voiceIndex)->value;
    }

    void sumVoiceBlocks() override {
        selectVoiceBlock();
    }
public:
    void setVoiceIndex(int voiceIndex) {
        if (voiceIndex < 0 || voiceIndex >= voiceOutputs.size()) {
            throw std::out_of_range("PolyDemuxOutput::setVoiceIndex: voiceIndex out of range");
        }
        _voiceIndex = voiceIndex;
    }
protected:
    int _voiceIndex;
};

#endif //PATCHCORE_POLYDEMUXOUTPUT_HPP
