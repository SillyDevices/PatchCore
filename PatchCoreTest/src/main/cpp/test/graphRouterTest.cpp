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

TEST(GraphRouterTest, OldRouterLatencyTest) {
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
