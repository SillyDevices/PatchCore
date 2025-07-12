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

#ifndef PATCHCORE_PAN_CROSSFADE_MODULE_HPP
#define PATCHCORE_PAN_CROSSFADE_MODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define PAN_CROSSFADE_MODULE_INPUT_INPUT "in"
#define PAN_CROSSFADE_MODULE_INPUT_CV "cv"
#define PAN_CROSSFADE_MODULE_OUTPUT_LEFT "outL"
#define PAN_CROSSFADE_MODULE_OUTPUT_RIGHT "outR"

class PanCrossFadeModule : public Module {
public:
    PanCrossFadeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    PanCrossFadeModule(std::string name);
    PanCrossFadeModule(const PanCrossFadeModule &other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~PanCrossFadeModule() = default;

    void envelope() override;
private:

    ModuleInput input = ModuleInput(PAN_CROSSFADE_MODULE_INPUT_INPUT);
    ModuleInput inputCv = ModuleInput(PAN_CROSSFADE_MODULE_INPUT_CV);
    ModuleOutput outputLeft = ModuleOutput(PAN_CROSSFADE_MODULE_OUTPUT_LEFT);
    ModuleOutput outputRight = ModuleOutput(PAN_CROSSFADE_MODULE_OUTPUT_RIGHT);
    FloatUserInput userInputCv = FloatUserInput(PAN_CROSSFADE_MODULE_INPUT_CV);
};

#endif //PATCHCORE_PAN_CROSSFADE_MODULE_HPP
