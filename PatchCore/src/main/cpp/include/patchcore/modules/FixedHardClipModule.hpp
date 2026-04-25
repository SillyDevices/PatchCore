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

#ifndef PATCHCORE_FIXED_HARD_CLIP_MODULE_HPP
#define PATCHCORE_FIXED_HARD_CLIP_MODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define FIXED_HARD_CLIP_MODULE_PARAMETER_THRESHOLD "threshold"

#define FIXED_HARD_CLIP_MODULE_INPUT "in"
#define FIXED_HARD_CLIP_MODULE_OUTPUT "out"

class FixedHardClipModule: public Module {
public:
    FixedHardClipModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    FixedHardClipModule(std::string name, float threshold = 1.0f);
    FixedHardClipModule(const FixedHardClipModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    void init();
    ~FixedHardClipModule() override = default;

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(FIXED_HARD_CLIP_MODULE_INPUT);
    ModuleOutput output = ModuleOutput(FIXED_HARD_CLIP_MODULE_OUTPUT);
protected:
    float threshold = 1.0f;
};

#endif //PATCHCORE_FIXED_HARD_CLIP_MODULE_HPP
