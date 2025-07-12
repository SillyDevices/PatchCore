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

#include "patchcore/test/ModularSynthTester.hpp"

void ModularSynthTester::testSynth(ModularSynth *synth, float expectedLeft, float expectedRight,
                                   int bufferSize, int samples, float tolerance) {
    auto bufferCycles = samples / bufferSize;
    if (bufferCycles <= 0) {
        bufferCycles = 1; // Ensure at least one cycle
    }
    std::pair<float, float> output;
    auto remainingSamples = samples;
    for (int cycle = 0; cycle < bufferCycles; ++cycle) {
        synth->onStartBuffer(bufferSize);
        for (int i = 0; i < bufferSize && remainingSamples > 0; ++i) {
            output = synth->computeSample();
            remainingSamples--;
        }
    }
    auto [left, right] = output;
    if (abs(left - expectedLeft) > tolerance || abs(right - expectedRight) > tolerance) {
        auto message = "Synth output does not match expected values: expected [" +
                      std::to_string(expectedLeft) + ", " + std::to_string(expectedRight) +
                      "], got [" + std::to_string(left) + ", " + std::to_string(right) + "]";
        throw std::runtime_error(message);
    }
}