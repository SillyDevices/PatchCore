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

#ifndef PATCHCORE_VCF_HPP
#define PATCHCORE_VCF_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/EnumUserInput.hpp"

#define VCF_INPUT_INPUT "in"
#define VCF_INPUT_CUTOFF "cut"
#define VCF_INPUT_RESONANCE "res"
#define VCF_OUTPUT_OUTPUT "out"
#define VCF_INPUT_TYPE "type"

class VCF: public Module {
public:
    VCF(std::string name, int sampleRate);
    virtual ~VCF() = default;

protected:
    ModuleInput input = ModuleInput(VCF_INPUT_INPUT);
    ModuleInput cutoff = ModuleInput(VCF_INPUT_CUTOFF);
    ModuleInput resonance = ModuleInput(VCF_INPUT_RESONANCE);

    ModuleOutput output = ModuleOutput(VCF_OUTPUT_OUTPUT);

    FloatUserInput userCutoff = FloatUserInput(VCF_INPUT_CUTOFF);
    FloatUserInput userResonance = FloatUserInput(VCF_INPUT_RESONANCE);

    EnumUserInput typeInput = EnumUserInput(VCF_INPUT_TYPE);
};

#endif //PATCHCORE_VCF_HPP
