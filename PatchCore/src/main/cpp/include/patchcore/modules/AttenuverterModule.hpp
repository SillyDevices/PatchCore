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

#ifndef PATCHCORE_ATTENUVERTERMODULE_HPP
#define PATCHCORE_ATTENUVERTERMODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define ATTENUVERTER_MODULE_PARAMETER_VALUE "value"

#define ATTENUVERTER_MODULE_INPUT_INPUT "in"
#define ATTENUVERTER_MODULE_INPUT_VALUE "control"
#define ATTENUVERTER_MODULE_OUTPUT_OUTPUT "out"

class AttenuverterModule : public Module {
public:
    AttenuverterModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    AttenuverterModule(std::string name, float value = 1.0f);
    AttenuverterModule(const AttenuverterModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~AttenuverterModule() = default;

    void envelope() override;
private:
    float defaultValue = 1.0f;
    ModuleInput input = ModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT, defaultValue);
    FloatUserInput value = FloatUserInput(ATTENUVERTER_MODULE_INPUT_VALUE);
    ModuleOutput output = ModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT);
};


#endif //PATCHCORE_ATTENUVERTERMODULE_HPP
