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

#ifndef PATCHCORE_ALLPASSFILTER_HPP
#define PATCHCORE_ALLPASSFILTER_HPP

#include "patchcore/modules/reverb/internal/common/denormilize.h"

class AllPassFilter {
public:
    AllPassFilter(int bufferSize) : bufferSize(bufferSize) {
        buffer = new float[bufferSize];
        for (int i = 0; i < bufferSize; i++) { buffer[i] = 0; }
    };

    virtual ~AllPassFilter() {
        delete[] buffer;
    };

    float envelope(float sample, float feedback) {
        float value = buffer[pointer];
        float toWrite = sample + (value * feedback);
        undenormalise(toWrite);
        buffer[pointer] = toWrite;
        pointer = (pointer + 1) % bufferSize;
        float result = value - sample;
        undenormalise(result);
        return result;
    };
private:
    int bufferSize;
    float* buffer;
    int pointer = 0;
};

#endif //PATCHCORE_ALLPASSFILTER_HPP
