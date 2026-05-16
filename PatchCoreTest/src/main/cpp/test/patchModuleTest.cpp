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
#include <patchcore/modules/VCAModule.hpp>
#include <patchcore/modules/AttenuverterModule.hpp>

static auto waveTableProvider = DefaultWaveTableProvider(44100);
static auto factory = DefaultModuleFactory(&waveTableProvider, nullptr);

TEST(PatchModuleTest, PatchModuleBasicOutputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(
            synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100))
            );

    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.8f) } } );
    patchModule->exposeOutput(patchModule->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT), "voiceOutput");

    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    std::pair<float, float> lastResult = { 0.0f, 0.0f };

    int countSamples = 10;
    synth->onStartBuffer(countSamples);
    for (int i = 0; i < countSamples; i++) {
        auto result = synth->computeSample();
        lastResult = result;
    }
    synth->onEndBuffer();

    ASSERT_FLOAT_EQ(lastResult.first, 0.8f);
    ASSERT_FLOAT_EQ(lastResult.second, 0.8f);

    delete synth;
}

TEST(PatchModuleTest, PatchModuleBasicInputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(
        synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100))
        );

    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const_p_one", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", {  } );
    patchModule->exposeInput(patchModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
    patchModule->exposeOutput(patchModule->getModule("vca")->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");
    patchModule->addPatch(
            patchModule->getModule("const_p_one")->getModuleOutput(CONST_MODULE_OUTPUT),
            patchModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_CV));

    synth->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.8f) } } );

    synth->addPatch(synth->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

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

TEST(PatchModuleTest, PatchModuleBasicCopyInputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = std::make_unique<PatchModule>(&factory, "voice", 44100);
    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const_p_one", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", {  } );
    patchModule->addPatch(
            patchModule->getModule("const_p_one")->getModuleOutput(CONST_MODULE_OUTPUT),
            patchModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_CV));
    patchModule->exposeInput(patchModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
    patchModule->exposeOutput(patchModule->getModule("vca")->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");

    auto clone = patchModule->clone();
    patchModule.reset();

    synth->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.8f) } } );
    synth->addModule(std::move(clone));

    synth->addPatch(synth->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

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

TEST(PatchModuleTest, interolatedUserInputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = std::make_unique<PatchModule>(&factory, "voice", 44100);

    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "mod", { } );
    patchModule->exposeOutput(patchModule->getModule("mod")->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), "voiceOutput");
    patchModule->addPatch(
            patchModule->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT),
            patchModule->getModule("mod")->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
    auto input = dynamic_cast<FloatUserInput*>(patchModule->getModule("mod")->getUserInput(ATTENUVERTER_MODULE_INPUT_VALUE));
    input->setValue(1.0f);

    synth->addModule(std::move(patchModule));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    std::pair<float, float> lastResult = { 0.0f, 0.0f };

    for (auto i =0; i < 100; i++) {
        int countSamples = 100;
        synth->onStartBuffer(countSamples);

        for (int sample= 0; sample < countSamples ; sample++) {
            auto result = synth->computeSample();
            lastResult = result;
        }

        synth->onEndBuffer();
    }

    ASSERT_NEAR(lastResult.first, 1.0f, 0.0001f);
    ASSERT_NEAR(lastResult.second, 1.0f, 0.0001f);

    delete synth;
}

