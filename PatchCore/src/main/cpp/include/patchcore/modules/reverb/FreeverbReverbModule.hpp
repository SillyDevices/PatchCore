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

#ifndef PATCHCORE_FREEVERBREVERBMODULE_HPP
#define PATCHCORE_FREEVERBREVERBMODULE_HPP

//based on https://ccrma.stanford.edu/~jos/pasp/Freeverb.html

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"

#include "patchcore/modules/reverb/internal/CombFilter.hpp"
#include "patchcore/modules/reverb/internal/AllPassFilter.hpp"


#define SIMPLE_REVERB_INPUT_INPUT "in"
#define SIMPLE_REVERB_INPUT_ROOM_SIZE "size"
#define SIMPLE_REVERB_INPUT_DAMPING "damping"
#define SIMPLE_REVERB_INPUT_LEVEL "level"
#define SIMPLE_REVERB_INPUT_FREEZE "freeze"
#define SIMPLE_REVERB_OUTPUT "out"


class FreeverbReverbModule: public Module {
public:
    FreeverbReverbModule(std::string name, int sampleRate);
    FreeverbReverbModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    FreeverbReverbModule(const FreeverbReverbModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~FreeverbReverbModule() = default;
    void envelope() override;
private:
    ModuleInput input = ModuleInput(SIMPLE_REVERB_INPUT_INPUT);
    FloatUserInput inputSize = FloatUserInput(SIMPLE_REVERB_INPUT_ROOM_SIZE);
    FloatUserInput inputDamping = FloatUserInput(SIMPLE_REVERB_INPUT_DAMPING);
    FloatUserInput inputLevel = FloatUserInput(SIMPLE_REVERB_INPUT_LEVEL);
    BoolUserInput inputFreeze = BoolUserInput(SIMPLE_REVERB_INPUT_FREEZE);
    ModuleOutput output = ModuleOutput(SIMPLE_REVERB_OUTPUT);
private:
    CombFilter comb[8] = {
            CombFilter( 1116),//25.3 ms
            CombFilter( 1188),//26.9 ms
            CombFilter( 1277),//28.9 ms
            CombFilter( 1356),//30.7 ms
            CombFilter( 1422),//32.3 ms
            CombFilter( 1491),//33.8 ms
            CombFilter( 1557),//35.3 ms
            CombFilter( 1617),//36.7 ms
    };
    AllPassFilter allPass[4] = {
            AllPassFilter( 556),//12.6 ms
            AllPassFilter( 441),//10.0 ms
            AllPassFilter( 341),//7.7 ms
            AllPassFilter( 225),//5.1 ms
    };
};

#endif //PATCHCORE_FREEVERBREVERBMODULE_HPP
