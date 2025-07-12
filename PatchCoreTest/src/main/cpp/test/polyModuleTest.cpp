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
#include <patchcore/module/PolyModule.hpp>

static auto factory = DefaultModuleFactory(44100);

TEST(PolyModuleTest, PatchModuleToPolyBasicOutputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth,nullptr);

    auto patchModule = std::make_unique<PolyModule>(&factory, "voice", 44100, 2);

    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.1f)}});
    patchModule->addOutput(patchModule->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT), "voiceOutput");
    synth->addModule(std::move(patchModule));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    std::pair<float, float> lastResult = {0.0f, 0.0f};

    int countSamples = 10;
    synth->onStartBuffer(countSamples);
    for (int i = 0;i < countSamples;i++) {
        auto result = synth->computeSample();
        lastResult = result;
    }
    synth->onEndBuffer();

    ASSERT_FLOAT_EQ(lastResult.first, 0.2f);
    ASSERT_FLOAT_EQ(lastResult.second, 0.2f);

    delete synth;
}


TEST(PolyModuleTest, PatchModuleToPolyBasicIOTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth,nullptr);

    auto polyModule = std::make_unique<PolyModule>(&factory, "voice", 44100, 2);

    polyModule->createModule(VCA_MODULE_TYPE_NAME, "vca", { } );

    auto patchModule = std::make_unique<PatchModule>(&factory, "const_wrapper", 44100);
    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const_input", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f)}});
    patchModule->addOutput(patchModule->getModule("const_input")->getModuleOutput(CONST_MODULE_OUTPUT), "constOutput");

    polyModule->addModule(std::move(patchModule));
    polyModule->addPatch(
            polyModule->getModule("const_wrapper")->getModuleOutput("constOutput"),
            polyModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_INPUT));

    polyModule->addInput(polyModule->getModule("vca")->getModuleInput(VCA_MODULE_INPUT_CV), "voiceCV");
    polyModule->addOutput(polyModule->getModule("vca")->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "voiceOutput");

    synth->createModule(CONST_MODULE_TYPE_NAME, "const_cv", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) }} );
    synth->addModule(std::move(polyModule));
    synth->addPatch(
            synth->getModule("const_cv")->getModuleOutput(CONST_MODULE_OUTPUT),
            synth->getModule("voice")->getModuleInput("voiceCV"));
    synth->addPatch(
            synth->getModule("voice")->getModuleOutput("voiceOutput"),
            synth->getModuleInput(MODULE_OUTPUT_INPUT));

    std::pair<float, float> lastResult = {0.0f, 0.0f};

    int countSamples = 10;
    synth->onStartBuffer(countSamples);
    for (int i = 0;i < countSamples;i++) {
        auto result = synth->computeSample();
        lastResult = result;
    }
    synth->onEndBuffer();

    ASSERT_FLOAT_EQ(lastResult.first, 1.0f);
    ASSERT_FLOAT_EQ(lastResult.second, 1.0f);

    delete synth;
}


TEST(PolyModuleTest, PatchModuleToPolyUserInputTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto polyCount = 3;

    auto patchModule = std::make_unique<PolyModule>(&factory, "voice", 44100, polyCount);
//    auto patchModule = std::make_unique<PatchModule>(&factory, "voice", 44100);

    patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
    patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "mod", { } );
    patchModule->addOutput(patchModule->getModule("mod")->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), "voiceOutput");
    patchModule->addPatch(
            patchModule->getModule("const")->getModuleOutput(CONST_MODULE_OUTPUT),
            patchModule->getModule("mod")->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));

    // DO NOT use it like this, it will not work
    //auto input = dynamic_cast<FloatUserInput*>(patchModule->getModule("mod")->getUserInput(MODULATION_INPUT_VALUE));
    //input->setValue(1.0f);

    patchModule->addUserInput(patchModule->getModule("mod")->getUserInput(ATTENUVERTER_MODULE_INPUT_VALUE), "modulationInput");
    auto input = dynamic_cast<FloatUserInput*>(patchModule->getUserInput("modulationInput"));
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

    ASSERT_NEAR(lastResult.first, polyCount*1.0f, 0.0001f);
    ASSERT_NEAR(lastResult.second, polyCount*1.0f, 0.0001f);

    delete synth;
}
