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
#include <android/log_macros.h>

#undef LOG_TAG
#define LOG_TAG "GraphRouterTest"



static auto waveTableProvider = DefaultWaveTableProvider(44100);
static auto factory = DefaultModuleFactory(&waveTableProvider, nullptr);

void create2SampleDelayPatch(PatchModule* patchModule) {
    auto constModule = patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) }});

    auto vca = patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", { });
    auto vcaCvBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaCvBias", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});

    auto passOutput = patchModule->createModule(PASS_MODULE_TYPE_NAME, "output", {} );
    patchModule->addOutput(passOutput->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");



    { //PATCHES

        // 1 sample delay
        patchModule->addPatch(constModule->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_INPUT));
        patchModule->addPatch(vcaCvBias->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_CV));

        // 2 sample delay
        patchModule->addPatch(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), passOutput->getModuleInput(PASS_MODULE_INPUT));

    }
}

void create9SampleDelayPatch(PatchModule* patchModule) {
    auto constModule = patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) }});

    auto vca = patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", { });
    auto vcaCvBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaCvBias", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});

    auto passStage1 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass1", {} );
    auto passStage2 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass2", {} );
    auto passStage3 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass3", {} );
    auto passStage4 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass4", {} );
    auto passStage5 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass5", {} );
    auto passStage6 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass6", {} );
    auto passStage7 = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass7", {} );


    auto passOutput = patchModule->createModule(PASS_MODULE_TYPE_NAME, "output", {} );
    patchModule->addOutput(passOutput->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");



    { //PATCHES

        // 1 sample delay
        patchModule->addPatch(constModule->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_INPUT));
        patchModule->addPatch(vcaCvBias->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_CV));

        // 2 sample delay
        patchModule->addPatch(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), passStage1->getModuleInput(PASS_MODULE_INPUT));
        // 3 sample delay
        patchModule->addPatch(passStage1->getModuleOutput(PASS_MODULE_OUTPUT), passStage2->getModuleInput(PASS_MODULE_INPUT));
        // 4 sample delay
        patchModule->addPatch(passStage2->getModuleOutput(PASS_MODULE_OUTPUT), passStage3->getModuleInput(PASS_MODULE_INPUT));
        // 5 sample delay
        patchModule->addPatch(passStage3->getModuleOutput(PASS_MODULE_OUTPUT), passStage4->getModuleInput(PASS_MODULE_INPUT));
        // 6 sample delay
        patchModule->addPatch(passStage4->getModuleOutput(PASS_MODULE_OUTPUT), passStage5->getModuleInput(PASS_MODULE_INPUT));
        // 7 sample delay
        patchModule->addPatch(passStage5->getModuleOutput(PASS_MODULE_OUTPUT), passStage6->getModuleInput(PASS_MODULE_INPUT));
        // 8 sample delay
        patchModule->addPatch(passStage6->getModuleOutput(PASS_MODULE_OUTPUT), passStage7->getModuleInput(PASS_MODULE_INPUT));

        // 9 sample delay
        patchModule->addPatch(passStage7->getModuleOutput(PASS_MODULE_OUTPUT), passOutput->getModuleInput(PASS_MODULE_INPUT));

    }
}

TEST(GraphRouterTest, GraphRouterZeroLatencyTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));

    create9SampleDelayPatch(patchModule);

    // plus one sample delay
    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        //format as "[first, second], "
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);

    delete synth;
}


TEST(GraphRouterTest, TwoModuleWithPatchInputFirstTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
    {
        auto vca = patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", {});
        //input added after modules is added to the router, so router don't mark it as "output module"!!! this is a problem!
        patchModule->addInput(vca->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
        patchModule->addOutput(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");
        //no adding any patches here, and it causes router have np patches and no modules to envelope

        auto vcaBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaBias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
        patchModule->addPatch(vcaBias->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_CV));
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}


TEST(GraphRouterTest, TwoModuleWithPatchInputLastTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
    {
        auto vca = patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", {});
        //input added after modules is added to the router, so router don't mark it as "output module"!!! this is a problem!
        //no adding any patches here, and it causes router have np patches and no modules to envelope

        auto vcaBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaBias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
        patchModule->addPatch(vcaBias->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_CV));

        patchModule->addInput(vca->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
        patchModule->addOutput(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}


TEST(GraphRouterTest, OneModuleWithPatchTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
    {
        auto pass = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass", {});
        //input added after modules is added to the router, so router don't mark it as "output module"!!! this is a problem!
        patchModule->addInput(pass->getModuleInput(PASS_MODULE_INPUT), "input");
        patchModule->addOutput(pass->getModuleOutput(PASS_MODULE_OUTPUT), "output");
        //no adding any patches here, and it causes router have np patches and no modules to envelope

        auto fakeConst = patchModule->createModule(CONST_MODULE_TYPE_NAME, "fakeConst", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
        auto fakePass = patchModule->createModule(PASS_MODULE_TYPE_NAME, "fakePass", {});
        patchModule->addPatch(fakeConst->getModuleOutput(CONST_MODULE_OUTPUT), fakePass->getModuleInput(PASS_MODULE_INPUT));
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}



TEST(GraphRouterTest, OneModuleTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
    {
        auto pass = patchModule->createModule(PASS_MODULE_TYPE_NAME, "pass", {});
        //input added after modules is added to the router, so router don't mark it as "output module"!!! this is a problem!
        patchModule->addInput(pass->getModuleInput(PASS_MODULE_INPUT), "input");
        patchModule->addOutput(pass->getModuleOutput(PASS_MODULE_OUTPUT), "output");
        //no adding any patches here, and it causes router have np patches and no modules to envelope
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}


TEST(GraphRouterTest, OneModuleInSynthTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );

    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    synth->onStartBuffer(samples);
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    for(int i = 0; i < samples; i++) {
        auto result = synth->computeSample();
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    }
    synth->onEndBuffer();

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}


