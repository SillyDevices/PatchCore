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


#import "ModularSynthWrapper.h"

#include <patchcore/module/factory/ModuleFactory.hpp>
#include <patchcore/synth/ModularSynth.hpp>


uintptr_t modularSynthCreate(uintptr_t module_factory_pointer, int sample_rate) {
    auto factory = reinterpret_cast<ModuleFactory *>(module_factory_pointer);
    ModularSynth* synth = new ModularSynth(factory, sample_rate);
    auto modulePtr = static_cast<Module *>(synth);
    auto synthPtr = reinterpret_cast<uintptr_t>(modulePtr);
    return synthPtr;
}
