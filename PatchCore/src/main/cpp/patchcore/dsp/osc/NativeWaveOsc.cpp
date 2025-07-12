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

#include "patchcore/dsp/osc/NativeWaveOsc.hpp"
#include <cmath>


float nativeSaw(float phase) {
    return 1.0f - 2.0f * phase;
}

float nativeSin(float phase) {
    return sinf(2 * (float) M_PI * phase);
}

float nativeSquare(float phase, float pwm) {
    if (phase <= pwm)
        return 1.0f;
    else
        return -1.0f;
}

float nativeTriangle(float phase, float pwm) {
    if (phase <= pwm)
        return 1.0f - 2.0f * ((1.0f / pwm) * phase);
    else
        return 1.0f - 2.0f * ((1.0f / (1 - pwm)) - (1.0f / (1.0f - pwm)) * phase);
}