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

#ifndef PATCHCORE_VCA_MODULE_HPP
#define PATCHCORE_VCA_MODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define VCA_MODULE_INPUT_INPUT      "in"
#define VCA_MODULE_INPUT_CV         "cv"
#define VCA_MODULE_OUTPUT_OUTPUT    "out"


class VCAModule: public Module{
public:
    VCAModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    explicit VCAModule(std::string name);
    VCAModule(const VCAModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~VCAModule() = default;

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(VCA_MODULE_INPUT_INPUT);
    ModuleInput cv = ModuleInput(VCA_MODULE_INPUT_CV);
    ModuleOutput output = ModuleOutput(VCA_MODULE_OUTPUT_OUTPUT);
};

#endif //PATCHCORE_VCA_MODULE_HPP
