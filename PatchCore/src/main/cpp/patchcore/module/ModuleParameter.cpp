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

#include "patchcore/module/ModuleParameter.hpp"


ModuleParameter::ModuleParameter(int value)
        : type(INT), intValue(value), floatValue(0.0f), boolValue(false) {}

ModuleParameter::ModuleParameter(float value)
        : type(FLOAT), intValue(0), floatValue(value), boolValue(false) {}

ModuleParameter::ModuleParameter(bool value)
        : type(BOOL), intValue(0), floatValue(0.0f), boolValue(value) {}

ModuleParameter::Type ModuleParameter::getType() {
    return type;
}

float ModuleParameter::getFloatValue() const {
    return floatValue;
}

int ModuleParameter::getIntValue() const {
    return intValue;
}

bool ModuleParameter::getBoolValue() const {
    return boolValue;
}



