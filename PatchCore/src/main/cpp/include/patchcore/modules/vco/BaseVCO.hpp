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

#ifndef PATCHCORE_BASEVCO_HPP
#define PATCHCORE_BASEVCO_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/module/TriggerModuleInput.hpp"
#include "patchcore/dsp/dsp.h"

#define VCO_INPUT_CV "cv"
#define VCO_INPUT_HZ "hz"
#define VCO_INPUT_RESET "reset"
#define VCO_OUTPUT_OUTPUT "out"

class BaseVCO: public Module{

private:
    class PhaseResetCallback: public TriggerModuleInput::OnTriggerCallback {
    public:
        PhaseResetCallback(BaseVCO *module): module(module) {}
        virtual ~PhaseResetCallback() = default;
        void onTrigger() override {
            module->reset();
        }
    private:
        BaseVCO *module;
    };

public:
    BaseVCO(std::string name, int sampleRate, float tune = dsp::tune_C0);
    virtual ~BaseVCO() = default;
public:
    void envelope() override;
public:
    bool isPeriodStart();
    virtual void reset();

protected:
    float computeFrequency(float voltage);
    void phaseIncrement(float frequency);
    void nextPeriod();
protected:
    float maxFrequency = sampleRate / 2.0f;

    float phase = .0f;
    bool periodStart = false;
    float tune;

    PhaseResetCallback phaseResetCallback = PhaseResetCallback(this);

    ModuleInput cv = ModuleInput(VCO_INPUT_CV);
    ModuleInput hz = ModuleInput(VCO_INPUT_HZ);
    FloatUserInput userCv = FloatUserInput(VCO_INPUT_CV);
    FloatUserInput userHz = FloatUserInput(VCO_INPUT_HZ);
    TriggerModuleInput resetPhaseInput = TriggerModuleInput(VCO_INPUT_RESET, &phaseResetCallback);
    ModuleOutput output = ModuleOutput(VCO_OUTPUT_OUTPUT);
};

#endif //PATCHCORE_BASEVCO_HPP
