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

#include "patchcore/modules/envelope/FadeEnvelopeModule.hpp"

#define FADER_THRESHOLD 0.5f

FadeEnvelopeModule::FadeEnvelopeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters):
        FadeEnvelopeModule(name, sampleRate,
                           ModuleParameter::getFloatOrDefault(parameters, FADE_ENVELOPE_MODULE_PARAMETER_TIME, 0.3f),
                           ModuleParameter::getBoolOrDefault(parameters, FADE_ENVELOPE_MODULE_PARAMETER_IS_OPENED, true)
                           ) {
}

FadeEnvelopeModule::FadeEnvelopeModule(std::string name, int sampleRate, float fadeTime, bool isOpened): Module(name, sampleRate) {
    init();
    fadeSamples = sampleRate*fadeTime;
    delta = 1.0f/((float)fadeSamples);
    if (isOpened){
        state = opened;
        outputValue = 1.0f;
    } else {
        state = closed;
        outputValue = 0.0f;
    }
}

FadeEnvelopeModule::FadeEnvelopeModule(const FadeEnvelopeModule &other): Module(other.name, other.sampleRate) {
    init();
    fadeSamples = other.fadeSamples;
    delta = other.delta;
    state = other.state;
    outputValue = other.outputValue;
    copyIOs(other);
}

std::unique_ptr<Module> FadeEnvelopeModule::clone() const {
    return std::make_unique<FadeEnvelopeModule>(*this);
}

void FadeEnvelopeModule::init() {
    registerUserInput(input);
    registerOutput(output);

}

void FadeEnvelopeModule::envelope() {
    switch (state) {
        case opened:
            if (input.value < FADER_THRESHOLD){
                state = closing;
            }
            outputValue = 1.0f;
            break;
        case closed:
            if (input.value > FADER_THRESHOLD){
                state = opening;
            }
            outputValue = 0.0f;
            break;
        case opening:
            outputValue = outputValue + delta;
            if (outputValue >= 1.0f) {
                outputValue = 1.0f;
                state = opened;
            }
            break;
        case closing:
            outputValue = outputValue - delta;
            if (outputValue <= 0.0f){
                outputValue = 0.0f;
                state = closed;
            }
            break;
    }
    output.value = outputValue;
}
