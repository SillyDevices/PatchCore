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

#include "patchcore/dsp/osc/BandLimitedTriangle.hpp"
#include <cmath>

#define M_PI_SQ 9.869604401089358f

float bandLimitedTriangleWithHarmonics(float pwm, float phase, int numHarmonics){
    float omegaT = 2 * M_PI * phase;
    float result = .0f;
    for (int i = 0; i < numHarmonics/2 + 1; i++){
        int n=2*i+1;
        result += powf(-1, i) * powf(n,-2.0f)*sinf(n * omegaT);
    }
    return 8/M_PI_SQ * result;
}
