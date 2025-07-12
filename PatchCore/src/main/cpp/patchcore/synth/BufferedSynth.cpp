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

#include "patchcore/synth/BufferedSynth.hpp"

void BufferedSynth::computeSamples(int count) {
    if (_bufferSize != count) {
        _leftOutputBuffer = std::vector<float>(count);
        _rightOutputBuffer = std::vector<float>(count);
    }
    for (auto step = 0; step < count; step++){
        auto sample = computeSample();
        _leftOutputBuffer[step] = sample.first;
        _rightOutputBuffer[step] = sample.second;
    }
}

std::vector<float>* BufferedSynth::getLeftChannelSamples() {
    return &_leftOutputBuffer;
}

std::vector<float>* BufferedSynth::getRightChannelSamples() {
    return &_rightOutputBuffer;
}

