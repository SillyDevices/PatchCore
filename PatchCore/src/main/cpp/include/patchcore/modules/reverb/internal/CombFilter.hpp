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

#ifndef PATCHCORE_COMBFILTER_HPP
#define PATCHCORE_COMBFILTER_HPP

#include "patchcore/modules/reverb/internal/common/denormilize.h"

class CombFilter {
public:
    CombFilter(int maxBufferSize, int delayInSamples): bufferSize(maxBufferSize), delay(delayInSamples) {
        buffer = new float[bufferSize];
        for (int i = 0; i < bufferSize; i++) { buffer[i] = 0; }
    };
    CombFilter(int delayInSamples): CombFilter(delayInSamples, delayInSamples) { };
    virtual ~CombFilter() {
        delete[] buffer;
    };
    float envelope(float sample, float damping, float feedback) {
        int readPointer = (writePointer + bufferSize - delay) % bufferSize;
        float value = buffer[readPointer];
        undenormalise(value);
        last = (value * (1.0f - damping)) + (last * damping * damping);
        //last = (value * (1.0f - damping)) + (last * damping);
        undenormalise(last);
        float toWrite = sample + (last * feedback);
        undenormalise(toWrite);
        buffer[writePointer] = toWrite;
        writePointer = (writePointer + 1) % bufferSize;
        return value;
    }
private:
    int bufferSize;
    int delay;
    float* buffer;
    int writePointer = 0;
    float last = 0;
};

#endif //PATCHCORE_COMBFILTER_HPP
