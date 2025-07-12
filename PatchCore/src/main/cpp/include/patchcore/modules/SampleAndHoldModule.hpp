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

#ifndef PATCHCORE_SAMPLE_AND_HOLD_MODULE_HPP
#define PATCHCORE_SAMPLE_AND_HOLD_MODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define SAMPLE_AND_HOLD_MODULE_INPUT "in"
#define SAMPLE_AND_HOLD_MODULE_INPUT_GATE "gate"
#define SAMPLE_AND_HOLD_MODULE_OUTPUT "out"

class SampleAndHoldModule : public Module {
public:
    SampleAndHoldModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    SampleAndHoldModule(std::string name);
    SampleAndHoldModule(const SampleAndHoldModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    void init();
    ~SampleAndHoldModule() override = default;

public:
    void envelope() override;
private:
    enum State {
        WAIT_FOR_RAISE, WAIT_FOR_FALL
    };
    State state = WAIT_FOR_RAISE;

    ModuleInput input = ModuleInput(SAMPLE_AND_HOLD_MODULE_INPUT);
    ModuleInput gate = ModuleInput(SAMPLE_AND_HOLD_MODULE_INPUT_GATE);
    ModuleOutput output = ModuleOutput(SAMPLE_AND_HOLD_MODULE_OUTPUT);
};


#endif //PATCHCORE_SAMPLE_AND_HOLD_MODULE_HPP
