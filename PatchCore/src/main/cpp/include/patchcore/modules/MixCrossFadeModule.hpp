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

#ifndef PATCHCORE_MIX_CROSSFADEMODULE_HPP
#define PATCHCORE_MIX_CROSSFADEMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define MIX_CROSSFADE_MODULE_INPUT_A "A"
#define MIX_CROSSFADE_MODULE_INPUT_B "B"
#define MIX_CROSSFADE_MODULE_INPUT_CV "cv"
#define MIX_CROSSFADE_MODULE_OUTPUT "out"

class MixCrossFadeModule : public Module {
public:
    MixCrossFadeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    MixCrossFadeModule(std::string name);
    MixCrossFadeModule(const MixCrossFadeModule &other);
    void init();
    virtual ~MixCrossFadeModule() = default;
    std::unique_ptr<Module> clone() const override;

public:
    void envelope() override;

protected:
    ModuleInput inputA = ModuleInput(MIX_CROSSFADE_MODULE_INPUT_A);
    ModuleInput inputB = ModuleInput(MIX_CROSSFADE_MODULE_INPUT_B);
    ModuleInput inputCv = ModuleInput(MIX_CROSSFADE_MODULE_INPUT_CV);
    ModuleOutput output = ModuleOutput(MIX_CROSSFADE_MODULE_OUTPUT);
    FloatUserInput userInputCv = FloatUserInput(MIX_CROSSFADE_MODULE_INPUT_CV);
};

#endif //PATCHCOR_MIX_CROSSFADEMODULE_HPP
