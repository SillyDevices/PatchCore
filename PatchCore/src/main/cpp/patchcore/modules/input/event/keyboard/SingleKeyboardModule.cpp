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

#include "patchcore/modules/input/event/keyboard/SingleKeyboardModule.hpp"
#include "patchcore/modules/input/event/keyboard/poly/PolyProxyKeyboardModule.hpp"


SingleKeyboardModule::SingleKeyboardModule(std::string name, int sampleRate)
    : KeyboardModule(name, sampleRate) {
    init();
}

SingleKeyboardModule::SingleKeyboardModule(const SingleKeyboardModule& other)
    : KeyboardModule(other.name, other.sampleRate) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> SingleKeyboardModule::clone() const {
    return std::make_unique<SingleKeyboardModule>(*this);
}

void SingleKeyboardModule::init() {
}

std::unique_ptr<PolyProxyModule> SingleKeyboardModule::createPolyModuleProxy(PolyModule *polyModule) const {
    return std::make_unique<PolyProxyKeyboardModule>(this, polyModule);
}

void SingleKeyboardModule::envelope() {
    bool multiTrigger = multiTriggerInput.getValue();
    std::lock_guard<std::mutex> lock(changesMutex);
    for (auto notePtr = noteChanges.begin(); notePtr < noteChanges.end(); notePtr++){
        if ((*notePtr)->sample <= syncOffset){
            if ((*notePtr)->isOn){
                _gate = KEYBOARD_GATE_ON_VALUE;
                _cv = (*notePtr)->cv;
                _velocity = (*notePtr)->velocity;
            } else {
                if (_cv == (*notePtr)->cv) {
                    _gate = KEYBOARD_GATE_OFF_VALUE;
                }
            }
            noteChanges.erase(notePtr);
        }
    }
    syncOffset += 1;
    cv.value = _cv;
    velocity.value = _velocity;
    if (_gate) {
        if (multiTrigger && _lastCv != _cv) {
            gate.value = KEYBOARD_GATE_OFF_VALUE;
        } else {
            gate.value = KEYBOARD_GATE_ON_VALUE;
        }
    } else {
        gate.value = KEYBOARD_GATE_OFF_VALUE;
    }
    _lastCv = _cv;
}
