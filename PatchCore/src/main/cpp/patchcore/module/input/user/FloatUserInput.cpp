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

#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/ExposedModuleFloatUserInput.hpp"
#include "patchcore/module/input/user/poly/PolyProxyFloatUserInput.hpp"

#include <cmath>

FloatUserInput::FloatUserInput(std::string name): UserInput(name, UserInputType::FLOAT) {};
FloatUserInput::FloatUserInput(std::string name, float speed): UserInput(name, UserInputType::FLOAT), speed(speed) {
    if (speed < 5.0f) {
        throw std::invalid_argument("speed must be at least 5.0f");
    }
};

void FloatUserInput::prepareBlock(const BlockContext& context) {
    (void) context;
    if (!std::isfinite(value[PATCHCORE_BLOCK_SIZE - 1])) {
        value.fill(0.0f);
    }
    startValue = value[PATCHCORE_BLOCK_SIZE - 1];
    if (isLocked) {
        targetValue = lastSetParameterLockValue;
    } else {
        targetValue = lastSetValue;
    }
    //TODO make it linear and not depend on buffer size
    auto ratio = (sampleRate/441) * speed;
    delta = (targetValue - startValue) / ratio;
    for (int sampleIndex = 0; sampleIndex < PATCHCORE_BLOCK_SIZE; ++sampleIndex) {
        value[sampleIndex] = startValue + delta * static_cast<float>(sampleIndex + 1);
    }
}

void FloatUserInput::clearParameterLock() {
    value.fill(lastSetValue);
    isLocked = false;
}

ExposedModuleUserInput *FloatUserInput::createExposed(const std::string &withName) {
    return new ExposedModuleFloatUserInput(withName, this);
}

std::unique_ptr<PolyProxyUserInput> FloatUserInput::createPolyProxy(std::vector<Module *> modulesToProxy) {
    return std::make_unique<PolyProxyFloatUserInput>(this, modulesToProxy);
}



