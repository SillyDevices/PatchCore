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

#include "patchcore/modules/AttenuverterModule.hpp"

AttenuverterModule::AttenuverterModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : AttenuverterModule(name, ModuleParameter::getFloatOrDefault(parameters, ATTENUVERTER_MODULE_PARAMETER_VALUE, 0.0f)) {
}

AttenuverterModule::AttenuverterModule(std::string name, float value)
    : Module(name, 0), defaultValue(value),
    input(ATTENUVERTER_MODULE_INPUT_INPUT, value),
    value(ATTENUVERTER_MODULE_INPUT_VALUE),
    output(ATTENUVERTER_MODULE_OUTPUT_OUTPUT) {
    init();
}

AttenuverterModule::AttenuverterModule(const AttenuverterModule& other)
    : Module(other.name, 0), defaultValue(other.defaultValue), input(other.input), value(other.value), output(other.output) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> AttenuverterModule::clone() const {
    return std::make_unique<AttenuverterModule>(*this);
}

void AttenuverterModule::init() {
    registerInput(input);
    registerOutput(output);
    registerUserInput(value);
}

void AttenuverterModule::envelope() {
    output.value = input.value * value.value;
}
