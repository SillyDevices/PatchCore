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

#include <patchcore/modules/DCFilterModule.hpp>
#include <cmath>

DCFilterModule::DCFilterModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : DCFilterModule(name,
                     sampleRate,
                     ModuleParameter::getFloatOrDefault(
                             parameters,
                             DCFILTER_MODULE_PARAMETER_CUTOFF,
                             DCFILTER_MODULE_DEFAULT_CUTOFF)) {}

DCFilterModule::DCFilterModule(std::string name, int sampleRate, float cutoffHz)
    : Module(name, sampleRate) {
    init();
    float rc = 1.0f / (2.0f * M_PI * cutoffHz);
    alpha = rc / (rc + (1.0f / sampleRate));
}

DCFilterModule::DCFilterModule(const DCFilterModule& other)
    : Module(other.name, other.sampleRate) {
    init();
    prevInputValue = other.prevInputValue;
    prevOutputValue = other.prevOutputValue;
    alpha = other.alpha;
    copyIOs(other);
}

std::unique_ptr<Module> DCFilterModule::clone() const {
    return std::make_unique<DCFilterModule>(*this);
}

void DCFilterModule::init() {
    registerInput(input);
    registerOutput(output);
}

void DCFilterModule::envelope() {
    auto inputValue = input.value;
    float outputValue = inputValue - prevInputValue + alpha * prevOutputValue;
    prevInputValue = inputValue;
    prevOutputValue = outputValue;
    output.value = outputValue;
}

