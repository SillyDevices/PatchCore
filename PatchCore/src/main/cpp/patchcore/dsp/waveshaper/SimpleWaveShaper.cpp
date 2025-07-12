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

#include "patchcore/dsp/waveshaper/SimpleWaveShaper.hpp"
#include <algorithm>

float simpleWaveShaper(float input, float cv) {
    auto limitedCv = std::max(-0.9f, std::min(cv, 0.9f));
    auto scaledInput = (input + cv) * 0.9f; //why is input + cv?
    auto k = 2.0f * limitedCv / (1.0f - limitedCv);
    return (1.0f + k) * scaledInput / (1 + k * abs(scaledInput));
}
