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

#include "patchcore/modules/input/cvGate/CvGateKeyboard.hpp"
#include <ctime>


CvGateKeyboard::CvGateKeyboard(std::string name, int sampleRate)
: AbstractKeyboard(name, sampleRate) {
}

CvGateKeyboard::CvGateKeyboard(const CvGateKeyboard &other)
: AbstractKeyboard(other.name, other.sampleRate) {
}

std::unique_ptr<Module> CvGateKeyboard::clone() const {
    return std::make_unique<CvGateKeyboard>(*this);
}

void CvGateKeyboard::envelope() {
    if (_gate) {
        gate.value = KEYBOARD_GATE_ON_VALUE;
        cv.value = _cv;
        velocity.value = _velocity;
    }
    else {
        gate.value = KEYBOARD_GATE_OFF_VALUE;
    }
}

void CvGateKeyboard::setValue(bool gate, float cv, float velocity) {
    _gate = gate ? KEYBOARD_GATE_ON_VALUE : KEYBOARD_GATE_OFF_VALUE;
    _cv = cv;
    _velocity = velocity;
}

std::unique_ptr<PolyProxyModule> CvGateKeyboard::createPolyModuleProxy(PolyModule *polyModule) const {
    throw std::runtime_error("SimpleKeyboard does not support polyphony");
}




