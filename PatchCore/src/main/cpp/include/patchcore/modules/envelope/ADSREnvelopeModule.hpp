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


#ifndef PATCHCORE_ADSR_ENVELOPE_MODULE_HPP
#define PATCHCORE_ADSR_ENVELOPE_MODULE_HPP

#include "BaseEnvelope.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define ENVELOPE_PARAMETER_DECAY_IS_RELEASE "decay_is_release"

#define ENVELOPE_INPUT_GATE "gate"

#define ADSR_INPUT_DELAY "dl"
#define ADSR_INPUT_ATTACK "a"
#define ADSR_INPUT_DECAY "d"
#define ADSR_INPUT_SUSTAIN "s"
#define ADSR_INPUT_RELEASE "r"

#define ADSR_INPUT_EXP_CURVE "curve_exp"

#define ADSR_INPUT_CYCLE "c"

#define ENVELOPE_OUTPUT_EXP "exp"


class ADSREnvelopeModule: public BaseEnvelope {
public:
    ADSREnvelopeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    ADSREnvelopeModule(std::string name, int sampleRate, bool decayIsRelease = false);
    ADSREnvelopeModule(const ADSREnvelopeModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~ADSREnvelopeModule() = default;

public:
    void internalEnvelope(bool gate) override;
protected:
    inline void resetTime() { t = .0f; };
protected:
    enum State {
        OFF, DELAY, ATTACK, DECAY, SUSTAIN, RELEASE, AFTER_RELEASE_ATTACK
    };
protected:
    FloatUserInput delay = FloatUserInput(ADSR_INPUT_DELAY);
    FloatUserInput attack = FloatUserInput(ADSR_INPUT_ATTACK);
    FloatUserInput decay = FloatUserInput(ADSR_INPUT_DECAY);
    FloatUserInput sustain = FloatUserInput(ADSR_INPUT_SUSTAIN);
    FloatUserInput release = FloatUserInput(ADSR_INPUT_RELEASE);
    FloatUserInput expCurve = FloatUserInput(ADSR_INPUT_EXP_CURVE);
    BoolUserInput cycle = BoolUserInput(ADSR_INPUT_CYCLE);

    ModuleInput moduleSustain = ModuleInput(ADSR_INPUT_SUSTAIN);
    ModuleOutput expOutput = ModuleOutput(ENVELOPE_OUTPUT_EXP);
protected:
    const float after_release_time = 0.001f;
    const float timeIncrement = 1.0f / sampleRate;
    float t = .0f;
    float releaseStart = .0f;
    float releaseStartExp = .0f;
    bool needReTrigger = false;
    State state = OFF;
    bool decayIsRelease = false;

protected:
    //output filter
    const float outputFilterParamter = pow(1 - 2*700.0f/(sampleRate), 2.0f);
    float outputFilterTemp = 0.0f;
    float expOutputFilterTemp = 0.0f;
};

#endif //PATCHCORE_ADSR_ENVELOPE_MODULE_HPP
