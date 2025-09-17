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



#include "factory/DefaultModuleFactoryWrapper.h"

#include "patchcore/module/factory/ModuleFactory.hpp"
#include "patchcore/module/factory/DefaultModuleFactory.hpp"


uintptr_t defaultModuleFactoryNew(uintptr_t waveTableProviderPointer, uintptr_t customModuleFactoryPointer){
    WaveTableProvider* waveTableProvider = reinterpret_cast<WaveTableProvider *>(waveTableProviderPointer);
    if (waveTableProvider == nullptr) {
        throw std::runtime_error("WaveTableProvider pointer is null");
    }
    ModuleFactory* customModuleFactory = reinterpret_cast<ModuleFactory *>(customModuleFactoryPointer);

    ModuleFactory *factory = new DefaultModuleFactory(waveTableProvider, customModuleFactory);
    uintptr_t result = reinterpret_cast<uintptr_t>(factory);
    return result;
}
