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



#include "PolyModuleWrapper.h"
#include "ModuleParameterWrapper.h"

#include <patchCore/module/factory/ModuleFactory.hpp>
#include <patchCore/module/PolyModule.hpp>

uintptr_t polyModuleNew(uintptr_t module_factory_pointer, char* name, int sample_rate, int polyphony) {
    auto *factory = reinterpret_cast<ModuleFactory *>(module_factory_pointer);
    if (factory == nullptr) throw std::runtime_error("ModuleFactory pointer is null");
    std::string nameString(name);
    Module *managedModule = new PolyModule(factory, nameString, sample_rate, polyphony);
    return reinterpret_cast<uintptr_t>(managedModule);
}

void polyModuleRelease(uintptr_t poly_module_pointer) {
    auto modulePtr = reinterpret_cast<Module *>(poly_module_pointer);
    if (modulePtr == nullptr) throw std::runtime_error("PolyModule pointer is null");
    delete modulePtr;
}
