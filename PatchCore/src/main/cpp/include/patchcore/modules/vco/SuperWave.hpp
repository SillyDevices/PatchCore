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

#ifndef PATCHCORE_SUPERWAVE_HPP
#define PATCHCORE_SUPERWAVE_HPP


#include "patchcore/dsp/wavetable/mipmap/MipmapWaveTable.hpp"
#include <cmath>

class SuperWave {
public:
    SuperWave(MipmapWaveTable *waveTable, int sampleRate) {
        this->sampleRate = sampleRate;
        this->waveTable = waveTable;
    }

    float envelope(float frequency, float spread) {
        if (spread <= 0.00001f) {
            spread = 0.0f;
        }
        auto freqAdd = spread * frequency * 0.25f;
        auto frequency0 = frequency;
        auto frequency1 = frequency + freqAdd;
        auto frequency2 = frequency - freqAdd;
        auto frequency3 = frequency1 + freqAdd;
        auto frequency4 = frequency2 - freqAdd;
        float result = 0.0f;
        auto phase1Shifted = phase1 + 0.15f;
        if (phase1Shifted > 1.0f) phase1Shifted -= 1.0f;
        auto phase2Shifted = phase2 + 0.25f;
        if (phase2Shifted > 1.0f) phase2Shifted -= 1.0f;
        auto phase3Shifted = phase3 + 0.45f;
        if (phase3Shifted > 1.0f) phase3Shifted -= 1.0f;
        auto phase4Shifted = phase4 + 0.70f;
        if (phase4Shifted > 1.0f) phase4Shifted -= 1.0f;
        result += waveTable->getSample(frequency0, phase0);
        result += waveTable->getSample(frequency1, phase1Shifted);
        result += waveTable->getSample(frequency2, phase2Shifted);
        result += waveTable->getSample(frequency3, phase3Shifted);
        result += waveTable->getSample(frequency4, phase4Shifted);
        phase0 += frequency0 / (float) sampleRate;
        phase1 += frequency1 / (float) sampleRate;
        phase2 += frequency2 / (float) sampleRate;
        phase3 += frequency3 / (float) sampleRate;
        phase4 += frequency4 / (float) sampleRate;
        phase0 = fmod(phase0, 1.0f);
        if (spread >= 0.00001f) {
            phase1 = fmod(phase1, 1.0f);
            phase2 = fmod(phase2, 1.0f);
            phase3 = fmod(phase3, 1.0f);
            phase4 = fmod(phase4, 1.0f);
        } else {
            phase1 = phase0;
            phase2 = phase0;
            phase3 = phase0;
            phase4 = phase0;
        }
        return result / 5.0f;
    }
    int sampleRate;
    MipmapWaveTable *waveTable;
    float phase0 = 0.0f;
    float phase1 = 0.0f;
    float phase2 = 0.0f;
    float phase3 = 0.0f;
    float phase4 = 0.0f;
};

#endif //PATCHCORE_SUPERWAVE_HPP
