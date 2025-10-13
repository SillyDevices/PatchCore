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

#include "patchcore/modules/vco/BaseVCO.hpp"
#include "patchcore/dsp/dsp.h"


BaseVCO::BaseVCO(std::string name, int sampleRate, float tune) : Module(name, sampleRate), tune(tune) {
    registerInput(cv);
    registerInput(hz);
    registerInput(resetPhaseInput);
    registerOutput(output);
    registerUserInput(userCv);
    registerUserInput(userHz);
}

bool BaseVCO::isPeriodStart() {
    return periodStart;
}

void BaseVCO::envelope() {
    resetPhaseInput.envelope();
}

void BaseVCO::reset() {
    phase = .0f;
    isPeriodStart();
}

float BaseVCO::computeFrequency(float voltage) {
    return dsp::voltToHz(tune, voltage);
}

void BaseVCO::phaseIncrement(float frequency) {
    periodStart = false;
    phase += frequency / (float) sampleRate;
    if (phase > 1.0f) nextPeriod();
    else if (phase < 0.0f) nextNegativePeriod();
}

void BaseVCO::nextPeriod() {
    periodStart = true;
    phase -= 1.0f;
}

void BaseVCO::nextNegativePeriod() {
    periodStart = true;
    phase += 1.0f;
}

