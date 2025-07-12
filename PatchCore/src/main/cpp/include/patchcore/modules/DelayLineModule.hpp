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

#ifndef PATCHCORE_DELAYLINEMODULE_HPP
#define PATCHCORE_DELAYLINEMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define DELAYLINE_MODULE_PARAMETER_MAX_TIME "maxTime"
#define DELAYLINE_MODULE_PARAMETER_INTERPOLATION "interpolation"

#define DELAYLINE_MODULE_INPUT_INPUT "in"
#define DELAYLINE_MODULE_INPUT_TIME "time"
#define DELAYLINE_MODULE_OUTPUT "out"

class DelayLineModule: public Module{
public:
    enum InterpolationType {
        NONE,
        LINEAR,
        QUADRATIC,
        CUBIC,
    };
public:
    DelayLineModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    DelayLineModule(std::string name, int sampleRate, float maxTime, InterpolationType interpolationType = QUADRATIC);
    DelayLineModule(const DelayLineModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~DelayLineModule();

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(DELAYLINE_MODULE_INPUT_INPUT);
    ModuleInput inputTime = ModuleInput(DELAYLINE_MODULE_INPUT_TIME);
    ModuleOutput output = ModuleOutput(DELAYLINE_MODULE_OUTPUT);
    FloatUserInput userInputTime = FloatUserInput(DELAYLINE_MODULE_INPUT_TIME, 200.0f);
private:
    InterpolationType type;
    float maxTime = 1.0f;
    int bufferSize = 1;
    float* buffer = nullptr;

    int writePointer = -1;
};


#endif //PATCHCORE_DELAYLINEMODULE_HPP
