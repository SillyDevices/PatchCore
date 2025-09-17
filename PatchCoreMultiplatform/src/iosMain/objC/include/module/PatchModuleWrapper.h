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


#ifndef PatchModuleWrapper_h
#define PatchModuleWrapper_h

#include <stdint.h>
#include "ModuleParameterWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t patchModuleNew(uintptr_t module_factory_pointer, char* name, int sample_rate);
void patchModuleDelete(uintptr_t patch_module_pointer);
uintptr_t patchModuleCreateModule(uintptr_t patch_module_pointer, char* module_type, char* module_name, ModuleParameterWrapper* parameters, unsigned long parameters_count);
void patchModuleAddModule(uintptr_t patch_module_pointer, uintptr_t managed_module_pointer);
uintptr_t patchModuleGetModule(uintptr_t patch_module_pointer, char* module_name);
void patchModuleAddInput(uintptr_t patch_module_pointer, uintptr_t input_pointer, char* input_name);
void patchModuleAddOutput(uintptr_t patch_module_pointer, uintptr_t output_pointer, char* output_name);
void patchModuleAddUserInput(uintptr_t patch_module_pointer, uintptr_t user_input_pointer, char* input_name);
void patchModuleAddPatch(uintptr_t patch_module_pointer, uintptr_t from_output_pointer, uintptr_t to_input_pointer);
void patchModuleResetPatch(uintptr_t patch_module_pointer);


#ifdef __cplusplus
}
#endif

#endif /* PatchModuleWrapper_h */
