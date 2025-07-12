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

#include "patchcore/modules/reverb/FreeverbReverbModule.hpp"

FreeverbReverbModule::FreeverbReverbModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters):
        FreeverbReverbModule(name, sampleRate) {

}

FreeverbReverbModule::FreeverbReverbModule(std::string name, int sampleRate) : Module(name, sampleRate) {
    init();
}

FreeverbReverbModule::FreeverbReverbModule(const FreeverbReverbModule &other): Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> FreeverbReverbModule::clone() const {
    return std::make_unique<FreeverbReverbModule>(*this);
}

void FreeverbReverbModule::init() {
    registerInput(input);
    registerUserInput(inputSize);
    registerUserInput(inputDamping);
    registerUserInput(inputLevel);
    registerUserInput(inputFreeze);
    registerOutput(output);
}

void FreeverbReverbModule::envelope() {
    if (inputFreeze.getValue()) {
        float out = 0;
        for (int i = 0; i < 8; i++) {
            out += comb[i].envelope(0, 0.0f, 1.0f);
        }
        for (int i = 0; i < 4; i++) {
            out = allPass[i].envelope(out, 0.5);
        }
        float outputValue = out * inputLevel.value;
        undenormalise(outputValue);
        output.value = outputValue;
    } else {
        float sample = input.value * 0.015f;
        float size = inputSize.value * 0.28f + 0.7f;
        float damping = inputDamping.value * 0.4f;

        float out = 0;
        for (int i = 0; i < 8; i++) {
            out += comb[i].envelope(sample, damping, size);
        }
        for (int i = 0; i < 4; i++) {
            out = allPass[i].envelope(out, 0.5);
        }
        float outputValue = out * inputLevel.value;
        undenormalise(outputValue);
        output.value = outputValue;
    }
}
