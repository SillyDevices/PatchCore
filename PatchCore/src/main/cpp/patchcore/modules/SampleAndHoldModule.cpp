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

#include "patchcore/modules/SampleAndHoldModule.hpp"

SampleAndHoldModule::SampleAndHoldModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
        : SampleAndHoldModule(name) {
}

SampleAndHoldModule::SampleAndHoldModule(std::string name) : Module(name, 0) {
    init();
}

SampleAndHoldModule::SampleAndHoldModule(const SampleAndHoldModule& other)
    : Module(other.name, other.sampleRate), state(other.state) {
    init();
    copyIOs(other);
}

[[nodiscard]] std::unique_ptr<Module> SampleAndHoldModule::clone() const {
    return std::make_unique<SampleAndHoldModule>(*this);
}

void SampleAndHoldModule::init() {
    registerInput(input);
    registerInput(gate);
    registerOutput(output);
}

void SampleAndHoldModule::envelope()  {
    if (gate.value > .5f){
        if (state == WAIT_FOR_RAISE) {
            output.value = input.value;
            state = WAIT_FOR_FALL;
        }
    } else {
        state = WAIT_FOR_RAISE;
    }
}
