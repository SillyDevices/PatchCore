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
#include "patchcore/module/input/user/ProxyModuleFloatUserInput.hpp"
#include "patchcore/module/input/user/poly/PolyProxyFloatUserInput.hpp"

FloatUserInput::FloatUserInput(std::string name): UserInput(name, UserInputType::FLOAT) {};
FloatUserInput::FloatUserInput(std::string name, float speed): UserInput(name, UserInputType::FLOAT), speed(speed) {};

void FloatUserInput::onStartBuffer(int bufferSize) {
    startValue = value;
    if (isLocked) {
        targetValue = lastSetParameterLockValue;
    } else {
        targetValue = lastSetValue;
    }
    value = startValue;
    //TODO make it linear and not depend on buffer size
    auto ratio = (sampleRate/441) * speed;
    delta = (targetValue - startValue) / ratio;
}

void FloatUserInput::envelope() {
    value += delta;
}

void FloatUserInput::clearParameterLock() {
    value = lastSetValue;
    isLocked = false;
}

ProxyModuleUserInput *FloatUserInput::createProxy(const std::string &withName) {
    return new ProxyModuleFloatUserInput(withName, this);
}

std::unique_ptr<PolyProxyUserInput> FloatUserInput::createPolyProxy(std::vector<Module *> modulesToProxy) {
    return std::make_unique<PolyProxyFloatUserInput>(this, modulesToProxy);
}




