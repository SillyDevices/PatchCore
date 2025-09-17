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


#ifndef ModuleWrapper_h
#define ModuleWrapper_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uintptr_t moduleGetModuleOutput(uintptr_t module_pointer, char* output_name);
uintptr_t moduleGetModuleInput(uintptr_t module_pointer, char* input_name);
uintptr_t moduleGetUserInput(uintptr_t module_pointer, char* input_name);


#ifdef __cplusplus
}
#endif


#endif /* ModuleWrapper_h */
