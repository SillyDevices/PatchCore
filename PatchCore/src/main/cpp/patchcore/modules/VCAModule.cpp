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

#include "patchcore/modules/VCAModule.hpp"
#include <algorithm>

VCAModule::VCAModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
        : VCAModule(name) {
}

VCAModule::VCAModule(std::string name) : Module(name, 0) {
    init();
}

VCAModule::VCAModule(const VCAModule& other)
    : Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

[[nodiscard]] std::unique_ptr<Module> VCAModule::clone() const {
    return std::make_unique<VCAModule>(*this);
}

void VCAModule::init() {
    registerInput(input);
    registerInput(cv);
    registerOutput(output);
}


void VCAModule::envelope() {
    auto val = input.value * cv.value;
    output.value = std::max(-1.0f, std::min(1.0f, val));
}
