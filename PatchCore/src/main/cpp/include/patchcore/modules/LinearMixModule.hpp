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

#ifndef PATCHCORE_LINEAR_MIX_MODULE_HPP
#define PATCHCORE_LINEAR_MIX_MODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define LINEAR_MIX_MODULE_INPUT_A   "A"
#define LINEAR_MIX_MODULE_INPUT_B   "B"
#define LINEAR_MIX_MODULE_INPUT_CV  "cv"
#define LINEAR_MIX_MODULE_OUTPUT    "out"

class LinearMixModule : public Module {
public:
    LinearMixModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    LinearMixModule(std::string name);
    LinearMixModule(const LinearMixModule &other);
    void init();
    virtual ~LinearMixModule() = default;
    std::unique_ptr<Module> clone() const override;

public:
    void envelope() override;

protected:
    ModuleInput inputA = ModuleInput(LINEAR_MIX_MODULE_INPUT_A);
    ModuleInput inputB = ModuleInput(LINEAR_MIX_MODULE_INPUT_B);
    ModuleInput inputCv = ModuleInput(LINEAR_MIX_MODULE_INPUT_CV);
    ModuleOutput output = ModuleOutput(LINEAR_MIX_MODULE_OUTPUT);
    FloatUserInput userInputCv = FloatUserInput(LINEAR_MIX_MODULE_INPUT_CV);
};


#endif //PATCHCORE_LINEAR_MIX_MODULE_HPP
