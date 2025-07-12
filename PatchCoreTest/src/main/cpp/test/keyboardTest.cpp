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

// changeLambda
void testOutputValue(ModularSynth* synth, float cvValue, std::function<void(float)> changeLambda) {
    std::pair<float, float> lastResult = {0.0f, 0.0f};

    int countSamples = 100;
    synth->onStartBuffer(countSamples);

    changeLambda(cvValue);

    for (int sample = 0; sample < countSamples; sample++) {
        auto result = synth->computeSample();
        lastResult = result;
    }

    synth->onEndBuffer();

    ASSERT_FLOAT_EQ(lastResult.first, cvValue);
    ASSERT_FLOAT_EQ(lastResult.second, cvValue);
}

TEST(KeyboardTest, monoKeyboardOnOffCvTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_CV), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    delete synth;
}

TEST(KeyboardTest, monoKeyboardOnOffGateTest) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_GATE), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 0.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    delete synth;
}


TEST(KeyboardTest, monoKeyboardTwoKeysCvTest1) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_CV), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 1.0f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 1.0f, 1.0f });
    });

    delete synth;
}

TEST(KeyboardTest, monoKeyboardTwoKeysGateTest1) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_GATE), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 1.0f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    testOutputValue(synth, 0.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 1.0f, 1.0f });
    });

    delete synth;
}

TEST(KeyboardTest, monoKeyboardTwoKeysCvTest2) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_CV), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 1.0f, 1.0f });
    });

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 1.0f, 1.0f });
    });

    testOutputValue(synth, 0.5f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    delete synth;
}

TEST(KeyboardTest, monoKeyboardTwoKeysGateTest2) {
    auto synth = new ModularSynth(&factory, 44100);
    ASSERT_NE(synth, nullptr);

    synth->createModule(KEYBOARD_MODULE_TYPE_NAME, "keyboard", {  } );

    synth->addPatch(synth->getModule("keyboard")->getModuleOutput(KEYBOARD_OUTPUT_GATE), synth->getModuleInput(MODULE_OUTPUT_INPUT));

    auto keyboard = dynamic_cast<KeyboardModule*>(synth->getModule("keyboard"));

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 0.5f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { true, 1.0f, 1.0f });
    });

    testOutputValue(synth, 1.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 1.0f, 1.0f });
    });

    testOutputValue(synth, 0.0f, [&](float cvValue) {
        keyboard->onEvent(KeyboardEvent { false, 0.5f, 1.0f });
    });

    delete synth;
}
