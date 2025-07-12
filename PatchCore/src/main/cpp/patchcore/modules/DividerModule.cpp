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

#include "patchcore/modules/DividerModule.hpp"

DividerModule::DividerModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : DividerModule(name, sampleRate) {}

DividerModule::DividerModule(std::string name, int sampleRate)
    : Module(name, sampleRate) {
    init();
}

DividerModule::DividerModule(const DividerModule& other)
    : Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> DividerModule::clone() const {
    return std::make_unique<DividerModule>(*this);
}

void DividerModule::init() {
    registerInput(num);
    registerInput(denum);
    registerOutput(output);
}

void DividerModule::envelope() {
    output.value = num.value / denum.value;
}

