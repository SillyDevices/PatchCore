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

#include "patchcore/modules/PanCrossFadeModule.hpp"


PanCrossFadeModule::PanCrossFadeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters):
        PanCrossFadeModule(name) {
}

PanCrossFadeModule::PanCrossFadeModule(std::string name) : Module(name, 0) {
    init();
}

PanCrossFadeModule::PanCrossFadeModule(const PanCrossFadeModule &other) : Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> PanCrossFadeModule::clone() const {
    return std::make_unique<PanCrossFadeModule>(*this);
}

void PanCrossFadeModule::init() {
    registerInput(input);
    registerInput(inputCv);
    registerOutput(outputLeft);
    registerOutput(outputRight);
    registerUserInput(userInputCv);
}


void PanCrossFadeModule::envelope() {
    // equal power panning
    auto limitedCv = std::max(-1.0f, std::min(1.0f, inputCv.value + userInputCv.value));
    auto cv = 0.5 * (limitedCv + 1);
    auto angle = cv * M_PI_2; // M_PI_2 is π/2
    auto amountLeft = std::cos(angle);
    auto amountRight = std::sin(angle);
    outputLeft.value = input.value * amountLeft;
    outputRight.value = input.value * amountRight;
}
