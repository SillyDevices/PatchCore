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

static auto factory = DefaultModuleFactory(44100);

TEST(SynthTest, synthCreation) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);
    delete synth;
}

TEST(SynthTest, synthTest) {
auto synth = new ModularSynth(&factory, 44100);
    int countSamples = 10;
    synth->onStartBuffer(countSamples);
    for (int i = 0; i < countSamples; i++) {
        auto result = synth->computeSample();
        ASSERT_FLOAT_EQ(result.first, 0.0f);
        ASSERT_FLOAT_EQ(result.second, 0.0f);
    }
    synth->onEndBuffer();
    delete synth;
}

TEST(SynthTest, synthTestOutput) {
    auto synth = new ModularSynth(&factory, 44100);
    synth->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.8f) } } );
    synth->addPatch(synth->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModuleInput(MODULE_OUTPUT_INPUT));
    int countSamples = 1;
    synth->onStartBuffer(countSamples);
    for (int i = 0; i < countSamples; i++) {
        auto result = synth->computeSample();
        ASSERT_FLOAT_EQ(result.first, 0.8f);
        ASSERT_FLOAT_EQ(result.second, 0.8f);
    }
    synth->onEndBuffer();
    delete synth;
}


TEST(SynthTest, synthTestVCAOutput) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.8f) } } );
    synth->createModule(CONST_MODULE_TYPE_NAME, "const_p_one", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    synth->createModule(VCA_MODULE_TYPE_NAME, "vca", {  } );
    synth->addPatch(
            synth->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT),
            synth->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_INPUT));
    synth->addPatch(
            synth->getModule("vca")->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT),
            synth->getModuleInput(MODULE_OUTPUT_INPUT));
    synth->addPatch(
            synth->getModule("const_p_one")->getModuleOutput(CONST_MODULE_OUTPUT),
            synth->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_CV));

    int countSamples = 10;
    synth->onStartBuffer(countSamples);
    std::pair<float, float> lastResult = {0.0f, 0.0f};
    for (int i = 0; i < countSamples; i++) {
        auto result = synth->computeSample();
        lastResult = result;
    }
    ASSERT_FLOAT_EQ(lastResult.first, 0.8f);
    ASSERT_FLOAT_EQ(lastResult.second, 0.8f);
    synth->onEndBuffer();
    delete synth;
}