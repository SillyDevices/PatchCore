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

#ifndef PATCHCORE_DIVIDER_MODULE_HPP
#define PATCHCORE_DIVIDER_MODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define DIVIDER_MODULE_INPUT_NUMERATOR "num"
#define DIVIDER_MODULE_INPUT_DENOMINATOR "denom"
#define DIVIDER_MODULE_OUTPUT_OUTPUT "out"

class DividerModule : public Module {
public:
    DividerModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    DividerModule(std::string name, int sampleRate);
    DividerModule(const DividerModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~DividerModule() = default;

public:
    void envelope() override;
private:
    ModuleInput num = ModuleInput(DIVIDER_MODULE_INPUT_NUMERATOR);
    ModuleInput denum = ModuleInput(DIVIDER_MODULE_INPUT_DENOMINATOR);
    ModuleOutput output = ModuleOutput(DIVIDER_MODULE_OUTPUT_OUTPUT);
};

#endif //PATCHCORE_DIVIDER_MODULE_HPP
