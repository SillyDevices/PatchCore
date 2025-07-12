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

#include "patchcore/dsp/wavetable/mipmap/SquareWaveTable.hpp"
#include <cmath>


SquareWaveTable::SquareWaveTable(SawWaveTable *table): MipmapWaveTable(0), sawWaveTable(table) {
    // Initialize this, because MipmapWaveTable destructor will delete these pointers
    lengths = new int[0];
    maxFrequency = new float[0];
    data = new float*[0];
}

float SquareWaveTable::getSample(float frequency, float pwm, float phase) {
    float phase1 = phase;
    float phase2 = phase + pwm;
    float saw1 =  sawWaveTable->getSample(frequency, phase1);
    float saw2 =  sawWaveTable->getSample(frequency, phase2);
    return (1 - 2*pwm) + (saw1 - saw2);
}

