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
    : BiQuadVCFModule(
            name,
            ModuleParameter::getFloatOrDefault(parameters, BIQUAD_VCF_PARAMETER_OUTPUT_GAIN, 0.5f),
            sampleRate) {

}

BiQuadVCFModule::BiQuadVCFModule(std::string name, float outputGain, int sampleRate)
    : VCF(name, sampleRate) {
    init();
    typeInput.setValue(LOWPASS);
    type = LOWPASS;
    this->outputGain = outputGain;
    userGain.setValue(0.f);

}

BiQuadVCFModule::BiQuadVCFModule(const BiQuadVCFModule& other)
    : VCF(other.name, other.sampleRate) {
    init();
    for (int i = 0; i < 3; ++i) b[i] = other.b[i];
    for (int i = 0; i < 2; ++i) a[i] = other.a[i];
    for (int i = 0; i < 2; ++i) x[i] = other.x[i];
    for (int i = 0; i < 2; ++i) y[i] = other.y[i];
    type = other.type;
    outputGain = other.outputGain;
    //typeInput.setValue(other.type);
    copyIOs(other);
}

std::unique_ptr<Module> BiQuadVCFModule::clone() const {
    return std::make_unique<BiQuadVCFModule>(*this);
}

void BiQuadVCFModule::init() {
    registerUserInput(userGain);
}

void BiQuadVCFModule::processSample(int sampleIndex) {
    processSampleInline(sampleIndex);
}

void BiQuadVCFModule::processBlock() {
    for (int sampleIndex = 0; sampleIndex < PATCHCORE_BLOCK_SIZE; ++sampleIndex) {
        processSampleInline(sampleIndex);
    }
}

inline void BiQuadVCFModule::processSampleInline(int sampleIndex) {
    float inputValue = input.value[sampleIndex];
    float cutoffValue = cutoff.value[sampleIndex] + userCutoff.value[sampleIndex];
    float resonanceValue = resonance.value[sampleIndex] + userResonance.value[sampleIndex];
    float gainValue = userGain.value[sampleIndex];

    float cutoffHz = dsp::voltToHz(dsp::tune_C0, cutoffValue);
    //temp fix for old xcode
//    float cutoffNorm = std::clamp(cutoffHz / (float) sampleRate, 0.001f, 0.499f);
    float cutoffNorm = cutoffHz / (float) sampleRate;
    if (cutoffNorm < 0.001f) {
        cutoffNorm = 0.001f;
    } else if (cutoffNorm > 0.499f) {
        cutoffNorm = 0.499f;
    }

    computeFilter(cutoffNorm, 0.1f + resonanceValue * 3.9f, gainValue);

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

    output.value[sampleIndex] = outputValue * outputGain;
}

void BiQuadVCFModule::computeFilter(float cutoff, float resonance, float gainDb) {
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
        case LOWSHELF: {
            // A = 10^(dB/40) - коэффициент усиления амплитуды
            float A = std::pow(10.0f, gainDb / 40.0f);
            float omega = 2.0f * M_PI * cutoff;
            float sn = std::sin(omega);
            float cs = std::cos(omega);
            // Q обычно фиксирован для полки (например, 0.707), но можно использовать resonance
            // beta = sqrt(A+A) для Q=0.707. Если хочешь управлять Q, формула: beta = sn / (2.0f * resonance) * sqrt((A*A + 1)/slope - ...)
            // Для упрощения возьмем "Slope = 1" (стандартная полка):
            float beta = sn * std::sqrt((A * A + 1.0f) / 1.0f - (std::pow(A - 1.0f, 2.0f)));

            float b0 = A * ((A + 1.0f) - (A - 1.0f) * cs + beta);
            float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cs);
            float b2 = A * ((A + 1.0f) - (A - 1.0f) * cs - beta);
            float a0 = (A + 1.0f) + (A - 1.0f) * cs + beta;
            float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cs);
            float a2 = (A + 1.0f) + (A - 1.0f) * cs - beta;

            // Нормализация сразу
            float norm = 1.0f / a0;
            this->b[0] = b0 * norm;
            this->b[1] = b1 * norm;
            this->b[2] = b2 * norm;
            this->a[0] = a1 * norm; // Внимание: в твоей реализации a[0] и a[1] используются с минусом в envelope
            this->a[1] = a2 * norm;
        } break;

        case HIGHSHELF: {
            // A = 10^(dB/40)
            float A = std::pow(10.0f, gainDb / 40.0f);
            float omega = 2.0f * M_PI * cutoff;
            float sn = std::sin(omega);
            float cs = std::cos(omega);
            // Используем fixed Q = 0.707 или берем из resonance
            // beta = sn * std::sqrt((A * A + 1.0f) / 1.0f - (std::pow(A - 1.0f, 2.0f)));
            // Упрощенная beta для slope=1 (Q=0.707):
            float beta = sn * std::sqrt(A);

            // Формулы High Shelf (зеркальны Low Shelf)
            float b0 = A * ((A + 1.0f) + (A - 1.0f) * cs + beta);
            float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cs);
            float b2 = A * ((A + 1.0f) + (A - 1.0f) * cs - beta);
            float a0 = (A + 1.0f) - (A - 1.0f) * cs + beta;
            float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cs);
            float a2 = (A + 1.0f) - (A - 1.0f) * cs - beta;

            float norm = 1.0f / a0;
            this->b[0] = b0 * norm;
            this->b[1] = b1 * norm;
            this->b[2] = b2 * norm;
            this->a[0] = a1 * norm;
            this->a[1] = a2 * norm;
        } break;

        case PEAKING: {
            float A = std::pow(10.0f, gainDb / 40.0f);
            float omega = 2.0f * M_PI * cutoff;
            float sn = std::sin(omega);
            float cs = std::cos(omega);
            // alpha = sin(w) / (2 * Q)
            float alpha = sn / (2.0f * resonance);

            float b0 = 1.0f + alpha * A;
            float b1 = -2.0f * cs;
            float b2 = 1.0f - alpha * A;
            float a0 = 1.0f + alpha / A;
            float a1 = -2.0f * cs;
            float a2 = 1.0f - alpha / A;

            float norm = 1.0f / a0;
            this->b[0] = b0 * norm;
            this->b[1] = b1 * norm;
            this->b[2] = b2 * norm;
            this->a[0] = a1 * norm;
            this->a[1] = a2 * norm;
        } break;

        case ALLPASS: {
            // 2-Pole AllPass (Gain игнорируется)
            float omega = 2.0f * M_PI * cutoff;
            float sn = std::sin(omega);
            float cs = std::cos(omega);
            float alpha = sn / (2.0f * resonance); // resonance здесь это Q

            // У AllPass числитель "зеркален" знаменателю
            float b0 = 1.0f - alpha;
            float b1 = -2.0f * cs;
            float b2 = 1.0f + alpha;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cs;
            float a2 = 1.0f - alpha;

            float norm = 1.0f / a0;
            this->b[0] = b0 * norm;
            this->b[1] = b1 * norm;
            this->b[2] = b2 * norm;
            this->a[0] = a1 * norm;
            this->a[1] = a2 * norm;
        } break;
    }
}

void BiQuadVCFModule::reset() {
    x[0] = 0;
    x[1] = 0;
    y[0] = 0;
    y[1] = 0;
}
