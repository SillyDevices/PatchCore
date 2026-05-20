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
#define LOG_TAG "PerformanceTest"

static auto waveTableProvider = DefaultWaveTableProvider(44100);
static auto factory = DefaultModuleFactory(&waveTableProvider, nullptr);

// For development purposes only: performance test for a patch module with multiple modules inside
//TEST(PerformanceTest, PatchModulePerformanceTest) {
//    auto synth = new ModularSynth(&factory, 44100);
//    ASSERT_NE(synth, nullptr);
//
//    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
//    {
//        auto passOutput = patchModule->createModule(PASS_MODULE_TYPE_NAME, "output", {} );
//
//        auto vco1 = patchModule->createModule(VCO_MODULE_TYPE_NAME, "vco1", {});
//        auto vco2 = patchModule->createModule(VCO_MODULE_TYPE_NAME, "vco2", {});
//        auto vco3 = patchModule->createModule(VCO_MODULE_TYPE_NAME, "vco3", {});
//        auto vco4 = patchModule->createModule(VCO_MODULE_TYPE_NAME, "vco4", {});
//
//        auto att1 = patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "att1", { {ATTENUVERTER_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});
//        auto att2 = patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "att2", { {ATTENUVERTER_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});
//        auto att3 = patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "att3", { {ATTENUVERTER_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});
//        auto att4 = patchModule->createModule(ATTENUVERTER_MODULE_TYPE_NAME, "att4", { {ATTENUVERTER_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});
//
//        auto mix12 = patchModule->createModule(LINEAR_MIX_MODULE_TYPE_NAME, "mix12", { });
//        auto mix34 = patchModule->createModule(LINEAR_MIX_MODULE_TYPE_NAME, "mix34", { });
//
//        patchModule->addOutput(passOutput->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");
//        {
//            patchModule->addPatch(vco1->getModuleOutput(VCO_OUTPUT_OUTPUT), att1->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
//            patchModule->addPatch(vco2->getModuleOutput(VCO_OUTPUT_OUTPUT), att2->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
//            patchModule->addPatch(vco3->getModuleOutput(VCO_OUTPUT_OUTPUT), att3->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
//            patchModule->addPatch(vco4->getModuleOutput(VCO_OUTPUT_OUTPUT), att4->getModuleInput(ATTENUVERTER_MODULE_INPUT_INPUT));
//
//            patchModule->addPatch(att1->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), mix12->getModuleInput(LINEAR_MIX_MODULE_INPUT_A));
//            patchModule->addPatch(att2->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), mix12->getModuleInput(LINEAR_MIX_MODULE_INPUT_B));
//
//            patchModule->addPatch(mix12->getModuleOutput(LINEAR_MIX_MODULE_OUTPUT), passOutput->getModuleInput(PASS_MODULE_INPUT));
//
////            patchModule->addPatch(att3->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), mix34->getModuleInput(LINEAR_MIX_MODULE_INPUT_A));
////            patchModule->addPatch(att4->getModuleOutput(ATTENUVERTER_MODULE_OUTPUT_OUTPUT), mix34->getModuleInput(LINEAR_MIX_MODULE_INPUT_B));
////            patchModule->addPatch(mix34->getModuleOutput(LINEAR_MIX_MODULE_OUTPUT), passOutput->getModuleInput(PASS_MODULE_INPUT));
//        }
//    }
//
//    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));
//
//    ALOGI("Starting performance test...\n");
//    auto repetitionCount = 10;
//    std::vector<long> durations(repetitionCount, 0) ;
//
//
//    for (int rep = 0; rep < repetitionCount; rep++) {
//
//        auto start = std::chrono::high_resolution_clock::now();
//
//        int countSamples = 44100 * 100; //100 seconds
//        for (int j = 0; j < countSamples;) {
//            for (int i = 0; i < 100; i++) {
//            }
//            j += 100;
//        }
//
//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        durations[rep] = duration.count();
//    }
//
//    ALOGI("----------------------------------------");
//    auto meanDuration = 0L;
//    for (int rep = 0; rep < repetitionCount; rep++) {
//        meanDuration += durations[rep];
//        ALOGI("Repetition %d: %ld ms", rep + 1, durations[rep]);
//    }
//    meanDuration /= repetitionCount;
//    ALOGI("----------------------------------------");
//    ALOGI("Mean duration: %ld ms", meanDuration);
//    ALOGI("----------------------------------------");
//
//    delete synth;
//}