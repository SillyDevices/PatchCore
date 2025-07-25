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

#import "ModuleWrapper.h"

#include <patchcore/module/Module.hpp>

uintptr_t moduleGetModuleOutput(uintptr_t module_pointer, NSString *output_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    std::string outputNameString = std::string([output_name UTF8String]);
    auto result = module->getModuleOutput(outputNameString);
    return reinterpret_cast<uintptr_t>(result);
}

uintptr_t moduleGetModuleInput(uintptr_t module_pointer, NSString *input_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    if (module == nullptr) throw std::runtime_error("Module pointer is null");
    std::string inputNameString([input_name UTF8String]);
    auto result = module->getModuleInput(inputNameString);
    return reinterpret_cast<uintptr_t>(result);
}

uintptr_t moduleGetUserInput(uintptr_t module_pointer, NSString *input_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    std::string inputNameString([input_name UTF8String]);
    auto result = module->getUserInput(inputNameString);
    auto pointerLong = reinterpret_cast<uintptr_t>(result);
    return pointerLong;
}
