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
#include "timeUtils.h"
#include <algorithm>

ModularSynth::ModularSynth(ModuleFactory *factory, int sampleRate) :
        AbstractSynth(),
        PatchModule(factory, "root", sampleRate) {
    registerInput(monoInput);
    registerInput(leftInput);
    registerInput(rightInput);
    //TODO maybe add inputs to PatchModule
}


void ModularSynth::computeBlock(StereoBlock& out) {
    BlockContext context;
    context.blockSize = PATCHCORE_BLOCK_SIZE;
    context.sampleRate = sampleRate;
    context.blockStartSample = currentBlockStartSample;
    context.blockStartTimeUs = getTimeUs();

    PatchModule::onStartBlock(context);
    PatchModule::processBlock();

    for (int index = 0; index < PATCHCORE_BLOCK_SIZE; ++index) {
        const float monoValue = monoInput.value[index];
        const float leftValue = leftInput.value[index];
        const float rightValue = rightInput.value[index];
        out.left[index] = leftValue + monoValue;
        out.right[index] = rightValue + monoValue;
    }

    currentBlockStartSample += PATCHCORE_BLOCK_SIZE;
}

void ModularSynth::staticRoutes() {

}
