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

#ifndef PATCHCORE_HALFRECTIFIERMODULE_HPP
#define PATCHCORE_HALFRECTIFIERMODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define HALFRECTIFIER_INPUT "in"
#define HALFRECTIFIER_OUTPUT "out"

class HalfRectifierModule : public Module {
public:
    HalfRectifierModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    explicit HalfRectifierModule(std::string name);
    HalfRectifierModule(const HalfRectifierModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~HalfRectifierModule() = default;

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(HALFRECTIFIER_INPUT);
    ModuleOutput output = ModuleOutput(HALFRECTIFIER_OUTPUT);
};


#endif //PATCHCORE_HALFRECTIFIERMODULE_HPP
