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

#ifndef PATCHCORE_PASS_MODULE_HPP
#define PATCHCORE_PASS_MODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define PASS_MODULE_INPUT "in"
#define PASS_MODULE_OUTPUT "out"

class PassModule: public Module {
public:
    PassModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    PassModule(std::string name);
    PassModule(const PassModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    void init();
    ~PassModule() override = default;

public:
    void envelope() override;
private:
    ModuleInput input = ModuleInput(PASS_MODULE_INPUT);
    ModuleOutput output = ModuleOutput(PASS_MODULE_OUTPUT);
};

#endif //PATCHCORE_PASS_MODULE_HPP
