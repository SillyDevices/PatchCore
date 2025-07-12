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

#ifndef PATCHCORE_EVENTKEYBOARD_HPP
#define PATCHCORE_EVENTKEYBOARD_HPP


#define KEYBOARD_INPUT_MULTI_TRIGGER "mt"

struct KeyboardEvent {
    bool press;
    float cv;
    float velocity;
};

class EventKeyboard {
public:
    virtual ~EventKeyboard() = default;
public:
    virtual void onEvent(KeyboardEvent event) = 0;
};

#endif //PATCHCORE_EVENTKEYBOARD_HPP
