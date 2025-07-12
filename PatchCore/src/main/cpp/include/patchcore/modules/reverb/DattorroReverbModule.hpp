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

#ifndef PATCHCORE_DATTORROREVERBMODULE_HPP
#define PATCHCORE_DATTORROREVERBMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"

#include "patchcore/modules/reverb/internal/CombFilter.hpp"
#include "patchcore/modules/reverb/internal/AllPassFilter.hpp"
#include "patchcore/modules/reverb/internal/dattorro/Diffuser.hpp"
#include "patchcore/modules/reverb/internal/dattorro/LowPass.hpp"

#define DATTORRO_REVERB_INPUT_INPUT_LEFT "inL"
#define DATTORRO_REVERB_INPUT_INPUT_RIGHT "inR"
#define DATTORRO_REVERB_INPUT_DIFFUSION "diffusion"
#define DATTORRO_REVERB_INPUT_ROOM_SIZE "size"
#define DATTORRO_REVERB_INPUT_DAMPING "damping"
#define DATTORRO_REVERB_OUTPUT_LEFT "outL"
#define DATTORRO_REVERB_OUTPUT_RIGHT "outR"

class DattorroReverbModule : public Module {
public:
    DattorroReverbModule(std::string name, int sampleRate);
    DattorroReverbModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    DattorroReverbModule(const DattorroReverbModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~DattorroReverbModule() = default;
    void envelope() override;

private:
    ModuleInput inputL = ModuleInput(DATTORRO_REVERB_INPUT_INPUT_LEFT);
    ModuleInput inputR = ModuleInput(DATTORRO_REVERB_INPUT_INPUT_RIGHT);
    FloatUserInput inputDiffusion = FloatUserInput(DATTORRO_REVERB_INPUT_DIFFUSION);
    FloatUserInput inputRoomSize = FloatUserInput(DATTORRO_REVERB_INPUT_ROOM_SIZE);
    FloatUserInput inputDamping = FloatUserInput(DATTORRO_REVERB_INPUT_DAMPING);
    ModuleOutput outputL = ModuleOutput(DATTORRO_REVERB_OUTPUT_LEFT);
    ModuleOutput outputR = ModuleOutput(DATTORRO_REVERB_OUTPUT_RIGHT);

    const float modulation_rate =  1.0f/3.f;
    float excursionAmplitude = 4.0f;
    float excursionPhase = 0.0f;
    float excursion_inc = modulation_rate/sampleRate;


    const float frequencyFactor = sampleRate/29761.0f;

//    Diffuser preDelay = Diffuser(2, 1);
    LowPass preDiffusersFilter = LowPass(0.9995);
    Diffuser diffuser0 = Diffuser(1 + 142*frequencyFactor, 142*frequencyFactor);
    Diffuser diffuser1 = Diffuser(1 + 107*frequencyFactor, 107*frequencyFactor);
    Diffuser diffuser2 = Diffuser(1 + 379*frequencyFactor, 379*frequencyFactor);
    Diffuser diffuser3 = Diffuser(1 + 277*frequencyFactor, 277*frequencyFactor);

    Diffuser tank0DecayDiffuser0 = Diffuser(1 + 672*frequencyFactor + int(excursionAmplitude*2), 672*frequencyFactor);
    Diffuser tank0MiddleDelay = Diffuser(1 + 4453*frequencyFactor, 4453*frequencyFactor, 1990*frequencyFactor, 353*frequencyFactor, 3627*frequencyFactor);
    LowPass tank0DampingFilter = LowPass(0.005);
    Diffuser tank0DecayDiffuser1 = Diffuser(1 + 1800*frequencyFactor, 1800*frequencyFactor, 187*frequencyFactor, 1228*frequencyFactor);
    Diffuser tank0EndDelay = Diffuser(1 + 3720*frequencyFactor, 3720*frequencyFactor, 1066*frequencyFactor, 2673*frequencyFactor);

    Diffuser tank1DecayDiffuser0 = Diffuser(1 + 908*frequencyFactor + int(excursionAmplitude*2), 908*frequencyFactor);
    Diffuser tank1MiddleDelay = Diffuser(1 + 4217*frequencyFactor, 4217*frequencyFactor, 266*frequencyFactor, 2574*frequencyFactor, 2111*frequencyFactor);
    LowPass tank1DampingFilter = LowPass(0.005);
    Diffuser tank1DecayDiffuser1 = Diffuser(1 + 2656*frequencyFactor,  2656*frequencyFactor, 1913*frequencyFactor, 335*frequencyFactor);
    Diffuser tank1EndDelay = Diffuser(1 + 3163*frequencyFactor,  3163*frequencyFactor, 1996*frequencyFactor, 121*frequencyFactor);

    float tank0Output = 0;
    float tank1Output = 0;

    Diffuser::Tap* leftTaps[7] = {
        tank1MiddleDelay.getTap(1),
        tank1MiddleDelay.getTap(2),
        tank1DecayDiffuser1.getTap(1),
        tank1EndDelay.getTap(1),
        tank0MiddleDelay.getTap(1),
        tank0DecayDiffuser1.getTap(1),
        tank0EndDelay.getTap(1)
    };
    Diffuser::Tap* rightTaps[7] = {
        tank0MiddleDelay.getTap(2),
        tank0MiddleDelay.getTap(3),
        tank0DecayDiffuser1.getTap(2),
        tank0EndDelay.getTap(2),
        tank1MiddleDelay.getTap(3),
        tank1DecayDiffuser1.getTap(2),
        tank1EndDelay.getTap(2)
    };

    // Damping and scaling factors
    //float feedbackGain = 0.7f;
    //float dampingFactor = 0.4f;
};

#endif // PATCHCORE_DATTORROREVERBMODULE_HPP
