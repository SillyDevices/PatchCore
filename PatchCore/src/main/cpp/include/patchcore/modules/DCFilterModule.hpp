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

#ifndef PATCHCORE_DCFILTERMODULE_HPP
#define PATCHCORE_DCFILTERMODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define DCFILTER_MODULE_INTPUT "in"
#define DCFILTER_MODULE_OUTPUT "out"

#define DCFILTER_MODULE_PARAMETER_CUTOFF "cutoff"

#define DCFILTER_MODULE_DEFAULT_CUTOFF 20.f


//namespace patchcore

class DCFilterModule: public Module {
public:
    DCFilterModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    DCFilterModule(std::string name, int sampleRate, float cutoffHz = DCFILTER_MODULE_DEFAULT_CUTOFF);
    DCFilterModule(const DCFilterModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~DCFilterModule() = default;

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(DCFILTER_MODULE_INTPUT);
    ModuleOutput output = ModuleOutput(DCFILTER_MODULE_OUTPUT);
private:
    float prevInputValue = 0.0f;
    float prevOutputValue = 0.0f;
    float alpha;
};

#endif //PATCHCORE_DCFILTERMODULE_HPP
