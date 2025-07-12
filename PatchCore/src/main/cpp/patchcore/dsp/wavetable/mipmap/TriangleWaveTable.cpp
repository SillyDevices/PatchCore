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

#include "patchcore/dsp/wavetable/mipmap/TriangleWaveTable.hpp"
#include "patchcore/dsp/osc/BandLimitedTriangle.hpp"
#include <cmath>


#define TABLE_COUNT 10


TriangleWaveTable::TriangleWaveTable(int sampleRate): MipmapWaveTable(TABLE_COUNT) {
    data = new float *[tableCount];
    maxFrequency = new float[tableCount];
    lengths = new int[tableCount];
    int harmonics[10] = {1024 ,512, 256, 128, 64, 32, 16, 8, 4, 1};
    int samples = 4096;
    for (int i = 0; i < tableCount; i++) {
        data[i] = new float[samples];
        maxFrequency[i] = float(sampleRate) / (harmonics[i] * 2);
        int h = harmonics[i] - 2;
        h = (h-1)/2;
        if (h < 1) h = 1;
        lengths[i] = samples;
        float phase = .0f;
        for (int j = 0; j < samples; j++) {
            data[i][j] = bandLimitedTriangleWithHarmonics(maxFrequency[i], phase, h);
            phase = j / ((float) samples);
        }
    }
}

TriangleWaveTable::TriangleWaveTable(TriangleWaveTable::noprapere_tag): MipmapWaveTable(TABLE_COUNT) {
    //do nothing
}
