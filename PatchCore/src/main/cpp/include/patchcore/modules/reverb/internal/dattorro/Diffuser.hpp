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

#ifndef PATCHCORE_DIFFUSER_HPP
#define PATCHCORE_DIFFUSER_HPP

#include <vector>
#include <algorithm>
#include "patchcore/dsp/dsp.h"
#include "patchcore/modules/reverb/internal/common/denormilize.h"

class Diffuser {
public:
    class Tap {
    public:
        Tap(int delay){
            this->delay = delay;
        }
    public:
        float envelope(float* buffer, int position, int bufferSize) {
            int readPosition = (position + bufferSize - delay) % bufferSize;
            value = buffer[readPosition];
            undenormalise_float(value);
            return value;
        }
    public:
        int delay;
        float value = 0;
    };
public:
    Diffuser(int bufferSize, int tap0delay, int tap1delay = 0, int tap2delay = 0, int tap3delay = 0) {
        _bufferSize = bufferSize;
        buffer = new float[_bufferSize];
        memset(buffer, 0, _bufferSize * sizeof(float));
        if (tap0delay > 0) {
            _taps.insert(_taps.begin(), Tap(tap0delay));
        }
        if (tap1delay > 0) {
            _taps.push_back(Tap(tap1delay));
        }
        if (tap2delay > 0) {
            _taps.push_back(Tap(tap2delay));
        }
        if (tap3delay > 0) {
            _taps.push_back(Tap(tap3delay));
        }
    }
    virtual ~Diffuser() {
        delete[] buffer;
    }

    inline double magnitudeTruncate(double value, double threshold = 1e-10) {
        return (fabs(value) < threshold) ? 0.0 : value;
    }

    float envelope(double input, double gain) {
        double delayed = _taps[0].envelope(buffer, writePointer, _bufferSize);
        for (int i = 1; i < _taps.size(); i++) {
            _taps[i].envelope(buffer, writePointer, _bufferSize);
        }
        writePointer = (writePointer + 1) % _bufferSize;
        float delayInput = magnitudeTruncate(input - delayed * gain);
        undenormalise_float(delayInput);
        buffer[writePointer] = delayInput;
        float result = magnitudeTruncate(delayed + delayInput * gain);
        undenormalise_float(result);
        return result;
    }
    float envelope(double input, double gain, float excursion) {
        float readPointer = writePointer - _taps[0].delay + excursion;
        while (readPointer < 0) { readPointer += _bufferSize; }
        int intReadPointer = (int)floorf(readPointer);
        int actualReadPointer = (intReadPointer + _bufferSize)%(_bufferSize);

        float fraction = readPointer - (float) intReadPointer;
        int nextReadPointer = intReadPointer + 1;
        if (nextReadPointer >= _bufferSize) { nextReadPointer = 0; }

        float delayed = dsp::linearInterpolation(buffer[actualReadPointer], buffer[nextReadPointer], fraction);

        for (int i = 1; i < _taps.size(); i++) {
            _taps[i].envelope(buffer, writePointer, _bufferSize);
        }
        writePointer = (writePointer + 1) % _bufferSize;
        float delayInput = magnitudeTruncate(input - delayed * gain);
        undenormalise_float(delayInput);
        buffer[writePointer] = delayInput;
        float result = magnitudeTruncate(delayed + delayInput * gain);
        undenormalise_float(result);
        return result;
    }
    Tap* getTap(int index) {
        return &(_taps[index]);
    }
private:
    int _bufferSize;
    float *buffer;
    int writePointer = 0;
    std::vector<Tap> _taps;
};

#endif //PATCHCORE_DIFFUSER_HPP
