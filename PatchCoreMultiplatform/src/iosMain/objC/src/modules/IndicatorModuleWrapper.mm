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


#import "modules/IndicatorModuleWrapper.h"

#import <Foundation/Foundation.h>

#include <patchcore/module/Module.hpp>
#include <patchcore/modules/IndicatorModule.hpp>


void setIndicatorBufferSize(uintptr_t pointer, int size) {
    auto *module = reinterpret_cast<Module *>(pointer);
    if (module == nullptr) throw std::runtime_error("IndicatorModuleJni.cpp: Module pointer is null");
    auto *indicatorModule = dynamic_cast<IndicatorModule *>(module);
    if (indicatorModule == nullptr) throw std::runtime_error("IndicatorModuleJni.cpp: IndicatorModule pointer is null or invalid");
    indicatorModule->setBufferSize(size);
}


int copyIndicatorBuffer(uintptr_t pointer, float* buffer, int size, int startIndex) {
    auto *module = reinterpret_cast<Module *>(pointer);
    if (module == nullptr) throw std::runtime_error("IndicatorModuleJni.cpp: Module pointer is null");
    auto *indicatorModule = dynamic_cast<IndicatorModule *>(module);
    if (indicatorModule == nullptr) throw std::runtime_error("IndicatorModuleJni.cpp: IndicatorModule pointer is null or invalid");
    auto count = indicatorModule->copyIntoBuffer(buffer, size, startIndex);
    return count;

}
