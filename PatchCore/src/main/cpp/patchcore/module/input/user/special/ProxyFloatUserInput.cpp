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

#include "patchcore/module/input/user/special/ProxyFloatUserInput.hpp"

ProxyFloatUserInput::ProxyFloatUserInput(std::string name, std::vector<FloatUserInput*> inputs) : FloatUserInput(name), inputs(std::move(inputs)) {
    count = this->inputs.size();
}

void ProxyFloatUserInput::setValue(float newValue) {
    value = newValue;
    for (int i = 0; i < count; i++){
        inputs[i]->setValue(newValue);
    }
}

void ProxyFloatUserInput::envelope() {
    for (int i = 0; i < count; i++){
        inputs[i]->envelope();
    }
}

void ProxyFloatUserInput::onStartBuffer(int size) {
    for (int i = 0; i < count; i++) {
        inputs[i]->onStartBuffer(size);
    }
}


