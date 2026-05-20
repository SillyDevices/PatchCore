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
#include "TestBlockUtils.hpp"
#include <patchcore/modules/ConstModule.hpp>
#include <patchcore/modules/VCAModule.hpp>
#include <patchcore/modules/AttenuverterModule.hpp>
#include <patchcore/modules/PassModule.hpp>
#include <patchcore/module/PolyModule.hpp>
#include <android/log_macros.h>
#include <utility>

#undef LOG_TAG
#define LOG_TAG "GraphRouterTest"



static auto waveTableProvider = DefaultWaveTableProvider(44100);
static auto factory = DefaultModuleFactory(&waveTableProvider, nullptr);

namespace {

#define TEST_STEP_MODULE_OUTPUT "out"

class TestStepModule: public Module {
public:
    explicit TestStepModule(std::string name): Module(std::move(name), 44100) {
        registerOutput(output);
    }

    TestStepModule(const TestStepModule& other): Module(other.name, other.sampleRate) {
        registerOutput(output);
        copyIOs(other);
    }

    [[nodiscard]] std::unique_ptr<Module> clone() const override {
        return std::make_unique<TestStepModule>(*this);
    }

    void processSample(int sampleIndex) override {
        output.value[sampleIndex] = sampleIndex == 0 ? 1.0f : 0.0f;
    }

private:
    ModuleOutput output = ModuleOutput(TEST_STEP_MODULE_OUTPUT);
};

class TestSampleProcessingProbeModule: public Module {
public:
    explicit TestSampleProcessingProbeModule(std::string name): Module(std::move(name), 44100) {
        registerInput(input);
        registerOutput(output);
    }

    TestSampleProcessingProbeModule(const TestSampleProcessingProbeModule& other): Module(other.name, other.sampleRate) {
        registerInput(input);
        registerOutput(output);
        processSampleCallCount = other.processSampleCallCount;
        processBlockCallCount = other.processBlockCallCount;
        copyIOs(other);
    }

    [[nodiscard]] std::unique_ptr<Module> clone() const override {
        return std::make_unique<TestSampleProcessingProbeModule>(*this);
    }

    void processSample(int sampleIndex) override {
        processSampleCallCount++;
        output.value[sampleIndex] = input.value[sampleIndex];
    }

    void processBlock() override {
        processBlockCallCount++;
        Module::processBlock();
    }

    int processSampleCallCount = 0;
    int processBlockCallCount = 0;

private:
    ModuleInput input = ModuleInput(PASS_MODULE_INPUT);
    ModuleOutput output = ModuleOutput(PASS_MODULE_OUTPUT);
};

} // namespace

void create2SampleDelayPatch(PatchModule* patchModule) {
    auto constModule = patchModule->createModule(CONST_MODULE_TYPE_NAME, "const", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) }});

    auto vca = patchModule->createModule(VCA_MODULE_TYPE_NAME, "vca", { });
    auto vcaCvBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaCvBias", { {CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) }});

    auto passOutput = patchModule->createModule(PASS_MODULE_TYPE_NAME, "output", {} );
    patchModule->exposeOutput(passOutput->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");



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
    patchModule->exposeOutput(passOutput->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");



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

TEST(GraphRouterTest, LoopStageDoesNotAddMoreThanOneSampleDelay) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto patchModule = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "voice", 44100)));
    ASSERT_NE(patchModule, nullptr);

    auto step = patchModule->addModule(std::make_unique<TestStepModule>("step"));
    auto passA = patchModule->createModule(PASS_MODULE_TYPE_NAME, "passA", {});
    auto passB = patchModule->createModule(PASS_MODULE_TYPE_NAME, "passB", {});
    ASSERT_NE(step, nullptr);
    ASSERT_NE(passA, nullptr);
    ASSERT_NE(passB, nullptr);

    patchModule->addPatch(step->getModuleOutput(TEST_STEP_MODULE_OUTPUT), passA->getModuleInput(PASS_MODULE_INPUT));
    patchModule->addPatch(passA->getModuleOutput(PASS_MODULE_OUTPUT), passB->getModuleInput(PASS_MODULE_INPUT));
    patchModule->addPatch(passB->getModuleOutput(PASS_MODULE_OUTPUT), passA->getModuleInput(PASS_MODULE_INPUT));
    patchModule->exposeOutput(passB->getModuleOutput(PASS_MODULE_OUTPUT), "voiceOutput");

    synth->addPatch(synth->getModule("voice")->getModuleOutput("voiceOutput"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto samples = PATCHCORE_BLOCK_SIZE + 2;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 1.0f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

    ASSERT_NE(firstResultSample, -1);
    ASSERT_LE(firstResultSample, 1) << "Looped SCC was processed with more than one sample of latency";

    delete synth;
}

TEST(GraphRouterTest, LoopStageWithPatchModuleDoesNotAddMoreThanOneSampleDelay) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto nestedPatch = dynamic_cast<PatchModule *>(synth->addModule(std::make_unique<PatchModule>(&factory, "nested", 44100)));
    ASSERT_NE(nestedPatch, nullptr);

    auto nestedProbe = dynamic_cast<TestSampleProcessingProbeModule *>(
        nestedPatch->addModule(std::make_unique<TestSampleProcessingProbeModule>("nestedProbe"))
    );
    ASSERT_NE(nestedProbe, nullptr);
    nestedPatch->exposeInput(nestedProbe->getModuleInput(PASS_MODULE_INPUT), "input");
    nestedPatch->exposeOutput(nestedProbe->getModuleOutput(PASS_MODULE_OUTPUT), "output");

    auto step = synth->addModule(std::make_unique<TestStepModule>("step"));
    auto feedback = synth->createModule(PASS_MODULE_TYPE_NAME, "feedback", {});
    ASSERT_NE(step, nullptr);
    ASSERT_NE(feedback, nullptr);

    synth->addPatch(step->getModuleOutput(TEST_STEP_MODULE_OUTPUT), nestedPatch->getModuleInput("input"));
    synth->addPatch(nestedPatch->getModuleOutput("output"), feedback->getModuleInput(PASS_MODULE_INPUT));
    synth->addPatch(feedback->getModuleOutput(PASS_MODULE_OUTPUT), nestedPatch->getModuleInput("input"));
    synth->addPatch(feedback->getModuleOutput(PASS_MODULE_OUTPUT), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto samples = PATCHCORE_BLOCK_SIZE + 2;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 1.0f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

    ASSERT_NE(firstResultSample, -1);
    ASSERT_LE(firstResultSample, 1) << "Looped SCC with PatchModule was processed with more than one sample of latency";
    ASSERT_GT(nestedProbe->processSampleCallCount, 0) << "Nested PatchModule in loop must process inner graph sample-by-sample";
    ASSERT_EQ(nestedProbe->processBlockCallCount, 0) << "Nested PatchModule in loop must not process inner graph through block API";

    delete synth;
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
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        //format as "[first, second], "
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

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
        patchModule->exposeInput(vca->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
        patchModule->exposeOutput(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");
        //no adding any patches here, and it causes router have np patches and no modules to envelope

        auto vcaBias = patchModule->createModule(CONST_MODULE_TYPE_NAME, "vcaBias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(1.0f) } } );
        patchModule->addPatch(vcaBias->getModuleOutput(CONST_MODULE_OUTPUT), vca->getModuleInput(VCA_MODULE_INPUT_CV));
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

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

        patchModule->exposeInput(vca->getModuleInput(VCA_MODULE_INPUT_INPUT), "input");
        patchModule->exposeOutput(vca->getModuleOutput(VCA_MODULE_OUTPUT_OUTPUT), "output");
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

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
        patchModule->exposeInput(pass->getModuleInput(PASS_MODULE_INPUT), "input");
        patchModule->exposeOutput(pass->getModuleOutput(PASS_MODULE_OUTPUT), "output");
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
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

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
        patchModule->exposeInput(pass->getModuleInput(PASS_MODULE_INPUT), "input");
        patchModule->exposeOutput(pass->getModuleOutput(PASS_MODULE_OUTPUT), "output");
        //no adding any patches here, and it causes router have np patches and no modules to envelope
    }

    auto bias = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );
    synth->addPatch(bias->getModuleOutput(CONST_MODULE_OUTPUT), synth->getModule("voice")->getModuleInput("input"));
    synth->addPatch(synth->getModule("voice")->getModuleOutput("output"), synth->getModuleInput(MODULE_OUTPUT_INPUT));


    auto samples = 20;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

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
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}

TEST(GraphRouterTest, Test) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    auto internal = std::make_unique<PolyModule>(&factory, "voice", 44100, 1);
    {
        auto passIn = internal->createModule(PASS_MODULE_TYPE_NAME, "passIn", {});
        internal->exposeInput(passIn->getModuleInput(PASS_MODULE_INPUT), "poly_input");
        auto passInternal = internal->createModule(PASS_MODULE_TYPE_NAME, "pass", {});
        internal->addPatch(passIn->getModuleOutput(PASS_MODULE_OUTPUT), passInternal->getModuleInput(PASS_MODULE_INPUT));
        internal->exposeOutput(passInternal->getModuleOutput(PASS_MODULE_OUTPUT), "poly_output_internal");
        auto passOut = internal->createModule(PASS_MODULE_TYPE_NAME, "passOut", {});
        internal->addPatch(passInternal->getModuleOutput(PASS_MODULE_OUTPUT), passOut->getModuleInput(PASS_MODULE_INPUT));
        internal->exposeOutput(passOut->getModuleOutput(PASS_MODULE_OUTPUT), "poly_output");
    }
    auto internalAdded = synth->addModule(std::move(internal));

    auto constModule = synth->createModule(CONST_MODULE_TYPE_NAME, "bias", { { CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );

    synth->addPatch(constModule->getModuleOutput(CONST_MODULE_OUTPUT), internalAdded->getModuleInput("poly_input"));
    synth->addPatch(internalAdded->getModuleOutput("poly_output_internal"), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto samples = 20;
    auto firstResultSample = -1;
    std::vector<std::pair<float, float>> results(samples, {0.0f, 0.0f});
    patchcore_test::computeSynthSamples(synth, samples, [&](int i, std::pair<float, float> result) {
        results[i] = result;
        if (result.first == 0.5f && firstResultSample == -1) {
            firstResultSample = i;
        }
    });

    ASSERT_EQ(firstResultSample, 0);
    ASSERT_EQ(results.size(), samples); // for debugging

    delete synth;
}


