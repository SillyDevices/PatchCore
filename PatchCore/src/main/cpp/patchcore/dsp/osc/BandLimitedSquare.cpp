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

#include "patchcore/dsp/osc/BandLimitedSquare.hpp"
#include "patchcore/dsp/osc/BandLimitedSaw.hpp"
#include <cmath>
#include <algorithm>


inline float bandLimitedSquare(float frequency, float pwm, float phase, int sampleRate) {
    return bandLimitedSquareWithMaxHarmonics(frequency, pwm, phase, sampleRate, 12);
}

float bandLimitedSquareWithHarmonics(float pwm, float phase, int numHarmonics){
    float omegaT = 2 * M_PI * phase;
    auto coefficients = new float[numHarmonics + 1];
    coefficients[0] = pwm - 0.5f;
    for (int i = 1; i < numHarmonics + 1; i++) {
        coefficients[i] = sinf(i * pwm * M_PI)* 2 / (i * (float) M_PI);
    }
    float result = coefficients[0];
    for (int i = 1; i < numHarmonics + 1; i++) {
        result += cosf(i * omegaT) * coefficients[i];
    }
    delete[] coefficients;
    return result;
}

float bandLimitedSquareWithMaxHarmonics(float frequency, float pwm, float phase, int sampleRate, int maxHarmonics){
    int numHarmonics = std::min((int) floorf((float)sampleRate / (frequency * 2)), maxHarmonics);
    float omegaT = 2 * M_PI * phase;
    auto coefficients = new float[numHarmonics + 1];
    coefficients[0] = pwm - 0.5f;
    for (int i = 1; i < numHarmonics + 1; i++) {
        coefficients[i] = sinf(i * pwm * M_PI)* 2 / (i * (float) M_PI);
    }
    float result = coefficients[0];
    for (int i = 1; i < numHarmonics + 1; i++) {
        result += cosf(i * omegaT) * coefficients[i];
    }
    delete[] coefficients;
    return result;
}