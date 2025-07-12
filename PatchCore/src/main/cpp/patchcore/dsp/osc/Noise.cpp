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

#include "patchcore/dsp/osc/Noise.hpp"
#include <cstdlib>


Noise::Noise() {
//    int randomCount = rand() % 1000;
//    for (int i = 0; i < randomCount; i++) {
//        envelope();
//    }
}

float Noise::envelope() {
    float _fLevel = 1.0f * g_fScale;
    g_x1 ^= g_x2;
    float result = g_x2 * _fLevel;
    g_x2 += g_x1;
    return result;
}

void Noise::reset() {
    //g_x1 = 0x67452301;
    //g_x2 = 0xefcdab89;
}
