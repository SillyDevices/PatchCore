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

#include "patchcore/modules/envelope/ADSREnvelopeModule.hpp"
#include <cmath>

ADSREnvelopeModule::ADSREnvelopeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : ADSREnvelopeModule(name, sampleRate, ModuleParameter::getBoolOrDefault(parameters, ENVELOPE_PARAMETER_DECAY_IS_RELEASE, false)) {}

ADSREnvelopeModule::ADSREnvelopeModule(std::string name, int sampleRate, bool decayIsRelease)
    : BaseEnvelope(name, sampleRate), decayIsRelease(decayIsRelease) {
    init();
}

ADSREnvelopeModule::ADSREnvelopeModule(const ADSREnvelopeModule& other)
    : BaseEnvelope(other.name, other.sampleRate), decayIsRelease(other.decayIsRelease) {
    init();
    this->t = other.t;
    this->releaseStart = other.releaseStart;
    this->releaseStartExp = other.releaseStartExp;
    this->needReTrigger = other.needReTrigger;
    this->state = other.state;
    this->outputFilterTemp = other.outputFilterTemp;
    this->expOutputFilterTemp = other.expOutputFilterTemp;
    copyIOs(other);
}

std::unique_ptr<Module> ADSREnvelopeModule::clone() const {
    return std::make_unique<ADSREnvelopeModule>(*this);
}

void ADSREnvelopeModule::init() {
    registerUserInput(delay);
    registerUserInput(attack);
    registerUserInput(decay);
    registerUserInput(sustain);
    registerUserInput(release);
    registerUserInput(expCurve);
    registerUserInput(cycle);
    registerInput(moduleSustain);
    registerOutput(expOutput);
}

void ADSREnvelopeModule::internalEnvelope(bool gate) {
    float outputValue = 0.0f;
    float expOutputValue = 0.0f;
    bool cycleValue = cycle.getValue();
    switch (state) {
        case OFF: {
            if (gate) {
                resetTime();
                state = DELAY;
            } else {
                outputValue = .0f;
                expOutputValue = .0f;
            }
            break;
        }
        case DELAY: {
            auto holdTime = delay.value;
            if (gate || cycleValue) {
                if (t <= holdTime) {
                    outputValue = .0f;
                    expOutputValue = .0f;
                } else {
                    resetTime();
                    needReTrigger = false;
                    state = ATTACK;
                }
            } else {
                resetTime();
                state = OFF;
            }
            break;
        }
        case ATTACK: {
            auto attackTime = attack.value;
            if (t <= attackTime) {
                auto expCoefficient = (expCurve.value < 0) ? (1.0f + expCurve.value) : (1.0f + expCurve.value * 9.0f);
                outputValue = 1.0f * t / attackTime;
                expOutputValue = 1.0f * pow(t / attackTime, expCoefficient);
//                expOutputValue = 1.0f * pow(t / attackTime, 0.5f);
                if (!gate) {
                    resetTime();
                    releaseStart = outputValue;
                    releaseStartExp = expOutputValue;
                    state = RELEASE;
                }
            } else {
                outputValue = 1.0f;
                expOutputValue = 1.0f;
                if (gate) {
                    state = DECAY;
                } else {
                    releaseStart = outputValue;
                    releaseStartExp = expOutputValue;
                    state = RELEASE;
                }
                resetTime();
            }
            break;
        }
        case DECAY: {
            float decayTime = decay.value;
            if (decayIsRelease) {
                decayTime = release.value;
            }
            float sustainLevel = sustain.value + moduleSustain.value;
            if (t <= decayTime) {
                outputValue = 1.0f - (1.0f - sustainLevel) * t / decayTime;
                auto expCoefficient = (expCurve.value < 0) ? (1.0f + expCurve.value) : (1.0f + expCurve.value * 9.0f);
                expOutputValue = 1.0f - (1.0f - sustainLevel) * pow(t / decayTime, expCoefficient);
//                expOutputValue = 1.0f - (1.0f - sustainLevel) * pow(t / decayTime, 0.5);
                if (!gate) {
                    if (sustainLevel <= 0.00001f) {
                        needReTrigger = true;
                        //no sustain do job
                    } else {
                        resetTime();
                        releaseStart = outputValue;
                        releaseStartExp = expOutputValue;
                        state = RELEASE;
                    }
                } else {
                    if (needReTrigger) {
                        resetTime();
                        releaseStart = outputValue;
                        releaseStartExp = expOutputValue;
                        state = AFTER_RELEASE_ATTACK;
                    }
                }
            } else {
                outputValue = sustainLevel;
                expOutputValue = sustainLevel;
                if (gate) {
                    state = SUSTAIN;
                } else {
                    releaseStart = outputValue;
                    releaseStartExp = expOutputValue;
                    state = RELEASE;
                }
            }
            break;
        }
        case SUSTAIN: {
            float sustainLevel = sustain.value + moduleSustain.value;
            outputValue = sustainLevel;
            expOutputValue = sustainLevel;
            if (!gate || cycleValue) {
                resetTime();
                releaseStart = outputValue;
                releaseStartExp = expOutputValue;
                state = RELEASE;
            }
            break;
        }
        case RELEASE: {
            float releaseTime = release.value;
            if (releaseTime < 0.0001f) {
                releaseTime = 0.0001f;
            }
            outputValue = releaseStart - releaseStart * t / releaseTime;
            auto expCoefficient = (expCurve.value < 0) ? (1.0f + expCurve.value) : (1.0f + expCurve.value * 9.0f);
            expOutputValue = releaseStartExp - releaseStartExp * pow(t / releaseTime, expCoefficient);
//            expOutputValue = releaseStartExp - releaseStartExp * pow(t / releaseTime, 0.5f);
            if (t >= releaseTime) {
                if (cycleValue && gate) {
                    resetTime();
                    state = DELAY;
                } else {
                    state = OFF;
                }
            }
            if (gate && !cycleValue) {
                resetTime();
                releaseStart = outputValue;
                releaseStartExp = expOutputValue;
                state = AFTER_RELEASE_ATTACK;
            }
            break;
        }
        case AFTER_RELEASE_ATTACK: {
            outputValue = releaseStart - 1 * pow(t / after_release_time, 1);
            expOutputValue = releaseStartExp - 1 * pow(t / after_release_time, 1);
            if (expOutputValue <= 0) expOutputValue = 0.0f;
            if (t >= after_release_time * releaseStart) {
                if (gate) {
                    resetTime();
                    state = ATTACK;
                    needReTrigger = false;
                } else {
                    state = OFF;
                }
            }
            break;
        }
    }
    t += timeIncrement;
    //filter
    outputValue = (1-outputFilterParamter) * outputValue + outputFilterParamter*outputFilterTemp;
    outputFilterTemp = outputValue;
    expOutputValue = (1-outputFilterParamter)*expOutputValue + outputFilterParamter*expOutputFilterTemp;
    expOutputFilterTemp = expOutputValue;
    //TODO NAN check
    output.value = outputValue;
    expOutput.value = expOutputValue;
}

