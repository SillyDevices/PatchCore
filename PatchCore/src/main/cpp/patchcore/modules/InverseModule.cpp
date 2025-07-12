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

#include "patchcore/modules/InverseModule.hpp"

InverseModule::InverseModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters):
        InverseModule(name) {
}

InverseModule::InverseModule(std::string name): Module(name, 0) {
    init();
}

InverseModule::InverseModule(const InverseModule &other): Module(other.name, 0) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> InverseModule::clone() const {
    return std::make_unique<InverseModule>(*this);
}

void InverseModule::init() {
    registerInput(input);
    registerOutput(output);
}

void InverseModule::envelope() {
    output.value = 1.0f / input.value;
}
