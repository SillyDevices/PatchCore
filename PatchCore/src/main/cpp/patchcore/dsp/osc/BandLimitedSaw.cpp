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

#include "patchcore/dsp/osc/BandLimitedSaw.hpp"
#include <cmath>
#include <algorithm>


inline float bandLimitedSaw(float frequency, float phase, int sampleRate){
    return bandLimitedSawWithMaxHarmonics(frequency, phase, sampleRate, 12);
}

float bandLimitedSawWithMaxHarmonics(float frequency, float phase, int sampleRate, int maxHarmonics){
    int numHarmonics = std::min((int) floorf((float) sampleRate / (frequency * 2)), maxHarmonics);
    if (numHarmonics % 2 == 1 && numHarmonics > 2)
        numHarmonics = numHarmonics - 1;
    float omegaT = 2 * M_PI * phase;
    auto coefficients = new float[numHarmonics + 1];
    coefficients[0] = 0.0f;
    for (int i = 1; i < numHarmonics + 1; i++) {
        coefficients[i] = powf(-1.0f, (float ) i) / (float) i;
    }
    float result = coefficients[0];
    for (int i = 1; i < numHarmonics + 1; i++) {
        result += coefficients[i] * sinf(i * omegaT);
    }
    delete[] coefficients;
    return 0.9f*2.0f*result/ (float) M_PI;
}

float bandLimitedSawWithHarmonics(float frequency, float phase, int numHarmonics){
    float omegaT = 2 * M_PI * phase;
    float result = .0f;
    for (int i = 1; i < numHarmonics + 1; i++){
        result += (1.0f/(float)i)*sinf(i*omegaT);
    }
    return 0.9*(2.0f/(float)M_PI)* result;
}