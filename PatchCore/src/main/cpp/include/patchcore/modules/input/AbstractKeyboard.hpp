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

#ifndef PATCHCORE_ABSTRACT_KEYBOARD_HPP
#define PATCHCORE_ABSTRACT_KEYBOARD_HPP


#include "patchcore/module/Module.hpp"

#define KEYBOARD_GATE_ON_VALUE 1.0f
#define KEYBOARD_GATE_OFF_VALUE 0.0f
#define KEYBOARD_GATE_THRESHOLD 0.5f


#define KEYBOARD_OUTPUT_GATE "gate"
#define KEYBOARD_OUTPUT_CV "cv"
#define KEYBOARD_OUTPUT_VELOCITY "velocity"

class AbstractKeyboard: public Module {
public:
    AbstractKeyboard(std::string name, int sampleRate);
    virtual ~AbstractKeyboard() = default;

protected:
    ModuleOutput gate = ModuleOutput(KEYBOARD_OUTPUT_GATE);
    ModuleOutput cv = ModuleOutput(KEYBOARD_OUTPUT_CV);
    ModuleOutput velocity = ModuleOutput(KEYBOARD_OUTPUT_VELOCITY);
};

#endif //PATCHCORE_ABSTRACT_KEYBOARD_HPP
