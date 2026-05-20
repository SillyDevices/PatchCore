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

#include "patchcore/synth/ModularSynth.hpp"

#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include "patchcore/modules/input/event/keyboard/SingleKeyboardModule.hpp"

#include "patchcore/dsp/dsp.h"
#include <algorithm>

ModularSynth::ModularSynth(ModuleFactory *factory, int sampleRate) :
        AbstractSynth(),
        PatchModule(factory, "root", sampleRate) {
    registerInput(monoInput);
    registerInput(leftInput);
    registerInput(rightInput);
    //TODO maybe add inputs to PatchModule
}


std::pair<float, float> ModularSynth::computeSample() {
    PatchModule::processSample(0);
    float leftValue = leftInput.value + monoInput.value;
    float rightValue = rightInput.value + monoInput.value;
    return { leftValue, rightValue };
}

void ModularSynth::computeBlock(StereoBlock& out) {
    PatchModule::processBlock();

    const auto& monoBuffer = monoInput.getBuffer();
    const auto& leftBuffer = leftInput.getBuffer();
    const auto& rightBuffer = rightInput.getBuffer();

    for (int index = 0; index < PATCHCORE_BLOCK_SIZE; ++index) {
        const float monoValue = index < static_cast<int>(monoBuffer.size()) ? monoBuffer[index] : monoInput.value;
        const float leftValue = index < static_cast<int>(leftBuffer.size()) ? leftBuffer[index] : leftInput.value;
        const float rightValue = index < static_cast<int>(rightBuffer.size()) ? rightBuffer[index] : rightInput.value;
        out.left[index] = leftValue + monoValue;
        out.right[index] = rightValue + monoValue;
    }
}

void ModularSynth::onStartBuffer(int size) {
    PatchModule::onStartBuffer(size);
}

void ModularSynth::onEndBuffer() {

}

void ModularSynth::staticRoutes() {

}
