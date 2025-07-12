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

#include "patchcore/modules/LinearMixModule.hpp"

LinearMixModule::LinearMixModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters):
        LinearMixModule(name) {
}

LinearMixModule::LinearMixModule(std::string name) : Module(name, 0) {
    init();
}

LinearMixModule::LinearMixModule(const LinearMixModule &other): Module(other.name, 0) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> LinearMixModule::clone() const {
    return std::make_unique<LinearMixModule>(*this);
}

void LinearMixModule::init(){
    registerInput(inputA);
    registerInput(inputB);
    registerInput(inputCv);
    registerOutput(output);
    registerUserInput(userInputCv);
}



void LinearMixModule::envelope() {
    auto cv = inputCv.value + userInputCv.value;

    auto amount1 = 1 - cv;
    auto amount2 = 1 + cv;
    output.value = (inputA.value * amount1 + inputB.value * amount2) / 2.0f;
//    equal power
//    auto angle = (cv + 1) * 0.25f * M_PI;
//    auto mixA = std::cos(angle);
//    auto mixB = std::sin(angle);
//    output.setValue((inputA.value * mixA + inputB.value * mixB));
}
