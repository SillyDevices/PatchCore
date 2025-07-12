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

#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include "timeUtils.h"


KeyboardModule::KeyboardModule(std::string name, int sampleRate)
    : AbstractKeyboard(name, sampleRate), EventKeyboard() {
    init();
}

KeyboardModule::KeyboardModule(const KeyboardModule& other)
    : AbstractKeyboard(other.name, other.sampleRate), EventKeyboard(other) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> KeyboardModule::clone() const {
    return std::make_unique<KeyboardModule>(*this);
}

void KeyboardModule::init() {
    registerOutput(gate);
    registerOutput(cv);
    registerOutput(velocity);
    registerUserInput(multiTriggerInput);
    registerUserInput(callbackInput);
}

void KeyboardModule::onStartBuffer(int size) {
    std::lock_guard<std::mutex> lock(changesMutex);
    syncTime = getTimeUs();
    if (noteChanges.size() > 0){
        for (auto notePtr = noteChanges.begin(); notePtr < noteChanges.end(); notePtr++){
            (*notePtr)->sample = (*notePtr)->sample - syncOffset;
        }
    }
    syncOffset = 0;
}

void KeyboardModule::envelope() {
    bool multiTrigger = multiTriggerInput.getValue();
    std::lock_guard<std::mutex> lock(changesMutex);
    for (auto notePtr = noteChanges.begin(); notePtr < noteChanges.end(); notePtr++){
        if ((*notePtr)->sample <= syncOffset){
            if ((*notePtr)->isOn){
                auto key = Key {
                        .cv = (*notePtr)->cv,
                        .velocity = (*notePtr)->velocity
                };
                pressedKeys.insert(pressedKeys.begin(), key);
            } else {
                for (auto pressedKeyPtr = pressedKeys.begin(); pressedKeyPtr < pressedKeys.end(); pressedKeyPtr++){
                    if ((*pressedKeyPtr).cv == (*notePtr)->cv){
                        pressedKeys.erase(pressedKeyPtr);
                    }
                }
            }
            noteChanges.erase(notePtr);
        }
    }
    syncOffset += 1;
    if (pressedKeys.empty()){
        gate.value = KEYBOARD_GATE_OFF_VALUE;
    } else {
        auto key = pressedKeys.at(0);
        float cvValue = key.cv;
        float velocityValue = key.velocity;
        if (multiTrigger && _lastCv != cvValue) {
            gate.value = KEYBOARD_GATE_OFF_VALUE;
        } else {
            gate.value = KEYBOARD_GATE_ON_VALUE;
        }
        _lastCv = cvValue;
        velocity.value = velocityValue;
        cv.value = cvValue;
    }
}

void KeyboardModule::onNoteOn(float cv, float velocity) {
    std::lock_guard<std::mutex> lock(changesMutex);
    if (syncTime < 0) {
        syncTime = getTimeUs();
    }
    double timeUs = getTimeUs();
    double dTime = timeUs - syncTime;
    int dSamples = dTime / sampleTime;

    auto change = new NoteChange {
            .sample = dSamples,
            .isOn = true,
            .cv = cv,
            .velocity = velocity
    };
    noteChanges.push_back(change);
}

void KeyboardModule::onNoteOff(float cv, float velocity) {
    std::lock_guard<std::mutex> lock(changesMutex);
    if (syncTime < 0) {
        syncTime = getTimeUs();
    }
    double timeUs = getTimeUs();
    double dTime = timeUs - syncTime;
    int dSamples = dTime / sampleTime;

    auto change = new NoteChange {
            .sample = dSamples,
            .isOn = false,
            .cv = cv,
            .velocity = velocity
    };

    noteChanges.push_back(change);
}

void KeyboardModule::onEvent(KeyboardEvent event) {
    if (event.press) {
        onNoteOn(event.cv, event.velocity);
    } else {
        onNoteOff(event.cv, event.velocity);
    }
}

