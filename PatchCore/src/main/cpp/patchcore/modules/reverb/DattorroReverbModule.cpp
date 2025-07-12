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

#include "patchcore/modules/reverb/DattorroReverbModule.hpp"

DattorroReverbModule::DattorroReverbModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
        : DattorroReverbModule(name, sampleRate) {}

DattorroReverbModule::DattorroReverbModule(std::string name, int sampleRate): Module(name, sampleRate) {
    init();
}

DattorroReverbModule::DattorroReverbModule(const DattorroReverbModule &other): Module(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> DattorroReverbModule::clone() const {
    return std::make_unique<DattorroReverbModule>(*this);
}

void DattorroReverbModule::init() {
    registerInput(inputL);
    registerInput(inputR);
    registerUserInput(inputRoomSize);
    registerUserInput(inputDamping);
    registerUserInput(inputDiffusion);
    registerOutput(outputL);
    registerOutput(outputR);
}

void DattorroReverbModule::envelope() {
    excursionPhase += excursion_inc;
    auto excursionValue = excursionAmplitude * sin(2 * (float) M_PI * excursionPhase);
    if (excursionPhase >= 1) { excursionPhase -= 1; }


    auto decay = inputRoomSize.value;
    auto damping = inputDamping.value; //0.0005f;
    auto bandwidth = inputDiffusion.value;// 0.9995f;

    preDiffusersFilter.setFeedback(1 - bandwidth);
    tank0DampingFilter.setFeedback(damping);
    tank1DampingFilter.setFeedback(damping);



    auto diffusers01 =  0.750f;
    auto diffusers23 = 0.625f;

    auto decayDiffuser0 = 0.7f; //inputDamping.value;
    auto decayDiffuser1 = std::max(std::min(decay + 0.25f, 0.5f), 0.25f);


//    auto preDelayOutput = preDelay.envelope(input.value, 0);
//    auto preDiffusersOutput = preDiffusersFilter.envelope(preDelayOutput);
    auto preDiffusersOutput = preDiffusersFilter.envelope(inputL.value + inputR.value);
    auto diffuser0Output = diffuser0.envelope(preDiffusersOutput, diffusers01);
    auto diffuser1Output = diffuser1.envelope(diffuser0Output, diffusers01);
    auto diffuser2Output = diffuser2.envelope(diffuser1Output, diffusers23);
    auto diffuser3Output = diffuser3.envelope(diffuser2Output, diffusers23);

    auto tankInput = diffuser3Output;

    auto tank0DecayDiffuser0Output = tank0DecayDiffuser0.envelope(tankInput + tank1Output, -decayDiffuser0, excursionValue);
    auto tank0MiddleDelayOutput = tank0MiddleDelay.envelope(tank0DecayDiffuser0Output, 0);
    auto tank0DampingFilterOutput = tank0DampingFilter.envelope(tank0MiddleDelayOutput) * decay;
    auto tank0DecayDiffuser1Output = tank0DecayDiffuser1.envelope(tank0DampingFilterOutput, decayDiffuser1);
    auto tank0EndDelayOutput = tank0EndDelay.envelope(tank0DecayDiffuser1Output, 0);

    auto tank1DecayDiffuser0Output = tank1DecayDiffuser0.envelope(tankInput + tank0Output, -decayDiffuser0, excursionValue);
    auto tank1MiddleDelayOutput = tank1MiddleDelay.envelope(tank1DecayDiffuser0Output, 0);
    auto tank1DampingFilterOutput = tank1DampingFilter.envelope(tank1MiddleDelayOutput) * decay;
    auto tank1DecayDiffuser1Output = tank1DecayDiffuser1.envelope(tank1DampingFilterOutput, decayDiffuser1);
    auto tank1EndDelayOutput = tank1EndDelay.envelope(tank1DecayDiffuser1Output, 0);

    tank0Output = tank0EndDelayOutput * decay;
    undenormalise(tank0Output);
    tank1Output = tank1EndDelayOutput * decay;
    undenormalise(tank1Output);

    //output and taps
    auto left =
            + 0.6f * leftTaps[0]->value +
            + 0.6f * leftTaps[1]->value +
            - 0.6f * leftTaps[2]->value +
            + 0.6f * leftTaps[3]->value +
            - 0.6f * leftTaps[4]->value +
            - 0.6f * leftTaps[5]->value +
            - 0.6f * leftTaps[6]->value;

    auto right =
            + 0.6f * rightTaps[0]->value +
            + 0.6f * rightTaps[1]->value +
            - 0.6f * rightTaps[2]->value +
            + 0.6f * rightTaps[3]->value +
            - 0.6f * rightTaps[4]->value +
            - 0.6f * rightTaps[5]->value +
            - 0.6f * rightTaps[6]->value;

    outputL.value = left;
    outputR.value = right;
}