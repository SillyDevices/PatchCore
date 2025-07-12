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

#ifndef PATCHCORE_DSP_H
#define PATCHCORE_DSP_H

#include <cmath>

namespace dsp {

    constexpr float tune_C0 = 16.3516f;

    inline float voltToHz(float tune, float voltage) {
        return tune * powf(2.0f, voltage);
    }

    inline float HzToVolt(float frequency) {
        return log2f(frequency);
    }

    inline float linearInterpolation(float y0, float y1, float dt) {
        return (y1 - y0) * dt + y0;
    }

    inline float quadraticInterpolation(float ym1, float y0, float y1, float dt) {
        float c2 = 0.5f * (y1 - 2.0f * y0 + ym1);
        float c1 = 0.5f * (y1 - ym1);
        float c0 = y0;
        return c2 * dt * dt + c1 * dt + c0;
    }

    inline float cubicInterpolation(float ym1, float y0, float y1, float y2, float dt) {
        float c3 = -ym1 + y0 - y1 + y2;
        float c2 = ym1 - y0 - c3;
        float c1 = y1 - ym1;
        float c0 = y0;
        return c3 * dt * dt * dt + c2 * dt * dt + c1 * dt + c0;
    }
}

#endif //PATCHCORE_DSP_H
