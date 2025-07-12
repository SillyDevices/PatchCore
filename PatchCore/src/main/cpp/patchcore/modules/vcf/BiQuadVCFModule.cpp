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

#include "patchcore/modules/vcf/BiQuadVCFModule.hpp"

#include "patchcore/dsp/dsp.h"

BiQuadVCFModule::BiQuadVCFModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : BiQuadVCFModule(name, sampleRate) {
}

BiQuadVCFModule::BiQuadVCFModule(std::string name, int sampleRate)
    : VCF(name, sampleRate) {
    init();
    typeInput.setValue(LOWPASS);
    type = LOWPASS;
}

BiQuadVCFModule::BiQuadVCFModule(const BiQuadVCFModule& other)
    : VCF(other.name, other.sampleRate) {
    init();
    for (int i = 0; i < 3; ++i) b[i] = other.b[i];
    for (int i = 0; i < 2; ++i) a[i] = other.a[i];
    for (int i = 0; i < 2; ++i) x[i] = other.x[i];
    for (int i = 0; i < 2; ++i) y[i] = other.y[i];
    type = other.type;
    //typeInput.setValue(other.type);
    copyIOs(other);
}

std::unique_ptr<Module> BiQuadVCFModule::clone() const {
    return std::make_unique<BiQuadVCFModule>(*this);
}

void BiQuadVCFModule::init() {
}

void BiQuadVCFModule::envelope() {
    float inputValue = input.value;
    float cutoffValue = cutoff.value + userCutoff.value;
    float resonanceValue = resonance.value + userResonance.value;

    float cutoffHz = dsp::voltToHz(dsp::tune_C0, cutoffValue);
    //temp fix for old xcode
//    float cutoffNorm = std::clamp(cutoffHz / (float) sampleRate, 0.001f, 0.499f);
    float cutoffNorm = cutoffHz / (float) sampleRate;
    if (cutoffNorm < 0.001f) {
        cutoffNorm = 0.001f;
    } else if (cutoffNorm > 0.499f) {
        cutoffNorm = 0.499f;
    }

    computeFilter(cutoffNorm, 0.1 + resonanceValue * 3.9);

    float outputValue =
            (b[0] * inputValue) +
            (b[1] * x[0]) +
            (b[2] * x[1]) -
            (a[0] * y[0]) -
            (a[1] * y[1]);

    x[1] = x[0];
    x[0] = inputValue;

    if (outputValue != outputValue) {
        outputValue = 0;
    }

    y[1] = y[0];
    y[0] = outputValue;

    output.value = outputValue * 0.5f;
}

void BiQuadVCFModule::computeFilter(float cutoff, float resonance) {
    Type newType = static_cast<Type>(typeInput.getValue());
    if (newType != type) {
        reset();
        type = newType;
    }
    switch (type) {
        case LOWPASS_1POLE: {
            a[0] = -std::exp(-2.f * M_PI * cutoff);
            a[1] = 0.f;
            b[0] = 1.f + this->a[0];
            b[1] = 0.f;
            b[2] = 0.f;
        } break;
        case HIGHPASS_1POLE: {
            a[0] = std::exp(-2.f * M_PI * (0.5f - cutoff));
            a[1] = 0.f;
            b[0] = 1.f - a[0];
            b[1] = 0.f;
            b[2] = 0.f;
        } break;
        case BANDPASS_1POLE: {
            float alphaLow = 1.f - std::exp(-2.f * M_PI * (cutoff - resonance));
            float alphaHigh = 1.f - std::exp(-2.f * M_PI * (cutoff + resonance));
            b[0] = alphaHigh - alphaLow;
            b[1] = 0.f;
            b[2] = 0.f;
            a[0] = -(1.f - alphaHigh - alphaLow);
            a[1] = 0.f;
        } break;
        case LOWPASS: {
            float K = std::tan(M_PI * cutoff);
            float norm = 1.f / (1.f + K / resonance + K * K);
            b[0] = K * K * norm;
            b[1] = 2.f * b[0];
            b[2] = b[0];
            a[0] = 2.f * (K * K - 1.f) * norm;
            a[1] = (1.f - K / resonance + K * K) * norm;
        } break;
        case HIGHPASS: {
            float K = std::tan(M_PI * cutoff);
            float norm = 1.f / (1.f + K / resonance + K * K);
            b[0] = norm;
            b[1] = -2.f * this->b[0];
            b[2] = this->b[0];
            a[0] = 2.f * (K * K - 1.f) * norm;
            a[1] = (1.f - K / resonance + K * K) * norm;
        } break;
        case BANDPASS: {
            float K = std::tan(M_PI * cutoff);
            float norm = 1.f / (1.f + K / resonance + K * K);
            b[0] = K / resonance * norm;
            b[1] = 0.f;
            b[2] = -b[0];
            a[0] = 2.f * (K * K - 1.f) * norm;
            a[1] = (1.f - K / resonance + K * K) * norm;
        } break;
        case NOTCH: {
            float omega = 2.f * M_PI * cutoff;
            float alpha = std::sin(omega) / (2.f * resonance);
            float norm = 1.f / (1.f + alpha);
            b[0] = 1.0f * norm;
            b[1] = -2.0f * std::cos(omega) * norm;
            b[2] = 1.0f * norm;
            a[0] = -2.0f * std::cos(omega) * norm;
            a[1] = (1.0f - alpha) * norm;
        } break;
    }
}

void BiQuadVCFModule::reset() {
    x[0] = 0;
    x[1] = 0;
    y[0] = 0;
    y[1] = 0;
}
