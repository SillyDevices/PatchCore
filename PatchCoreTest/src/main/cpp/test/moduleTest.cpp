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

#include <gtest/gtest.h>
#include <patchcore/module/factory/DefaultModuleFactory.hpp>
#include <patchcore/synth/ModularSynth.hpp>
#include <patchcore/modules/ConstModule.hpp>
#include <patchcore/modules/AttenuverterModule.hpp>

static auto factory = DefaultModuleFactory(44100);

TEST(ModuleTest, interolatedUserInputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    synth->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "mod", { });
    synth->addPatch(
            synth->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT),
            synth->getModule("mod")->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
    synth->addPatch(
            synth->getModule("mod")->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT),
            synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto input = dynamic_cast<FloatUserInput*>(
            synth->getModule("mod")->getUserInput(ATTENUVERTER_MODULE_INPUT_VALUE)
            );
    input->setValue(1.0f);

    std::pair<float, float> lastResult = {0.0f, 0.0f};

    for (auto i =0; i < 100; i++) {
        int countSamples = 100;
        synth->onStartBuffer(countSamples);

        for (int sample = 0; sample < countSamples; sample++) {
            auto result = synth->computeSample();
            lastResult = result;
        }

        synth->onEndBuffer();
    }

    ASSERT_NEAR(lastResult.first, 1.0f, 0.0001f);
    ASSERT_NEAR(lastResult.second, 1.0f, 0.0001f);

    delete synth;
}

