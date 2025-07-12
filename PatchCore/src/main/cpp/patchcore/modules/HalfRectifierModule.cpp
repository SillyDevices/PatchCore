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

#include "patchcore/modules/HalfRectifierModule.hpp"

HalfRectifierModule::HalfRectifierModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
        : HalfRectifierModule(name) {
}

HalfRectifierModule::HalfRectifierModule(std::string name)
    : Module(name, 44100) {
    init();
}

HalfRectifierModule::HalfRectifierModule(const HalfRectifierModule& other)
    : Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> HalfRectifierModule::clone() const {
    return std::make_unique<HalfRectifierModule>(*this);
}

void HalfRectifierModule::init() {
    registerInput(input);
    registerOutput(output);
}

void HalfRectifierModule::envelope() {
    auto inputValue = input.value;
    if (inputValue < 0) inputValue = 0;
    output.value = inputValue;
}
