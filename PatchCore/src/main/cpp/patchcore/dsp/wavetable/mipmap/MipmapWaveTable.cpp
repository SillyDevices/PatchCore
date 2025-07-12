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

#include "patchcore/dsp/wavetable/mipmap/MipmapWaveTable.hpp"
#include <cmath>

MipmapWaveTable::MipmapWaveTable(int count): tableCount(count) {}

float MipmapWaveTable::getSample(float frequency, float phase) {
    int tableNumber = tableCount - 1;
    for (int i = 0; i < tableCount; i++) {
        if (frequency < maxFrequency[i]) {
            tableNumber = i;
            break;
        }
    }
    int len = lengths[tableNumber];
    float floatIndex = phase * (float)(len);
    while (floatIndex >= len) { //make test for this, when floatIndex 4095.6 and len 4096
        floatIndex -= len;
    } // after that floatIndex is 4095.6
    while (floatIndex < 0) {
        floatIndex += len;
    }
    int index = nearbyint(floatIndex); // and index is 4096
    float fraction = floatIndex - (float) index;
    index = index % len; // added this for the case when index is 4096
    int nextIndex = (index + 1) % len;
    return fraction *  data[tableNumber][nextIndex] + (1.0f - fraction) *  data[tableNumber][index];
}

MipmapWaveTable::~MipmapWaveTable() {
    for (int i =0; i < tableCount; i++) {
        delete data[i];
    }
    delete lengths;
    delete data;
    delete maxFrequency;
}
