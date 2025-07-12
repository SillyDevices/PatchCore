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

#ifndef PATCHCORE_MODULAR_SYNTH_H
#define PATCHCORE_MODULAR_SYNTH_H

#include "patchcore/synth/AbstractSynth.hpp"

#include "patchcore/module/factory/ModuleFactory.hpp"
#include "patchcore/module/PatchModule.hpp"
#include "patchcore/module/Module.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/Router.hpp"

#include "patchcore/modules/input/event/keyboard/EventKeyboard.hpp"

#include <thread>

#define MODULE_OUTPUT_INPUT "mono"
#define MODULE_OUTPUT_LEFT_INPUT "left"
#define MODULE_OUTPUT_RIGHT_INPUT "right"

//namespace PatchCore {

class ModularSynth: public AbstractSynth, public PatchModule {
public:
    //TODO
    virtual ~ModularSynth() {};
public:
    ModularSynth(ModuleFactory *factory, int sampleRate);

protected:
    virtual void staticRoutes();

public:
    void onStartBuffer(int size) override;
    void onEndBuffer() override;
    std::pair<float, float> computeSample() override;

public:
    ModuleInput* getMonoOutput() {
        return &monoInput;
    }

    ModuleInput* getLeftOutput() {
        return &leftInput;
    }

    ModuleInput* getRightOutput() {
        return &rightInput;
    }
protected:
    //OutputModule *_output = new OutputModule("output");
    ModuleInput monoInput = ModuleInput(MODULE_OUTPUT_INPUT);
    ModuleInput leftInput = ModuleInput(MODULE_OUTPUT_LEFT_INPUT);
    ModuleInput rightInput = ModuleInput(MODULE_OUTPUT_RIGHT_INPUT);
};

//}

#endif //PATCHCORE_MODULAR_SYNTH_H
