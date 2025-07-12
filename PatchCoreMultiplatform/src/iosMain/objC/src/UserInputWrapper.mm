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


#import <Foundation/Foundation.h>

#import "UserInputWrapper.h"

#include <patchcore/module/input/user/FloatUserInput.hpp>
#include <patchcore/module/input/user/EnumUserInput.hpp>
#include <patchcore/module/input/user/BoolUserInput.hpp>


void floatUserInputSetValue(uintptr_t user_input_pointer, float value) {
    auto userInput = reinterpret_cast<UserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    auto floatUserInput = reinterpret_cast<FloatUserInput *>(userInput);
    if (floatUserInput == nullptr) throw std::runtime_error("UserInput is not a FloatUserInput");
    floatUserInput->setValue(value);
}

void enumUserInputSetValue(uintptr_t user_input_pointer, int value) {
    auto userInput = reinterpret_cast<EnumUserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    userInput->setValue(value);
}

void boolUserInputSetValue(uintptr_t user_input_pointer, bool value) {
    auto userInput = reinterpret_cast<BoolUserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    userInput->setValue(value);
}
