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

#include "patchcore/dsp/wavetable/DefaultWaveTableProvider.hpp"


DefaultWaveTableProvider::DefaultWaveTableProvider(int SampleRate) {
    sawTable = new SawWaveTable(SampleRate);
    triangleWaveTable = new TriangleWaveTable(SampleRate);
    squareWaveTable = new SquareWaveTable(sawTable);
    simpleSquareWaveTable = new SimpleSquareWaveTable(SampleRate);
}

DefaultWaveTableProvider::~DefaultWaveTableProvider() {
    delete sawTable;
    delete triangleWaveTable;
    delete squareWaveTable;
    delete simpleSquareWaveTable;
}

SawWaveTable *DefaultWaveTableProvider::getSawWaveTable() {
    return sawTable;
}

TriangleWaveTable *DefaultWaveTableProvider::getTriangleWaveTable() {
    return triangleWaveTable;
}

SquareWaveTable *DefaultWaveTableProvider::getSquareWaveTable() {
    return squareWaveTable;
}

SimpleSquareWaveTable *DefaultWaveTableProvider::getSimpleSquareWaveTable() {
    return simpleSquareWaveTable;
}
