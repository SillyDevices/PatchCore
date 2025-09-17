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

package com.sillydevices.patchcore.platform.module

import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.internal.pointers.ModuleOutputPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.module.factory.ModuleParameter


expect object PlatformPatchModule {

    fun new(moduleFactoryPointer: ModuleFactoryPointer, name: String, sampleRate: Int): ModulePointer
    fun release(patchModulePointer: ModulePointer)


    fun createModule(patchModulePointer: ModulePointer, moduleType: String, moduleName: String, parameters: List<ModuleParameter>): ModulePointer
    //fun AddModule(patchModulePointer: ModulePointer, modulePointer: ModulePointer)
    //fun GetModule(patchModulePointer: ModulePointer, moduleName: String): ULong

    fun addInput(patchModulePointer: ModulePointer, inputPointer: ModuleInputPointer, withName: String)
    fun addOutput(patchModulePointer: ModulePointer, outputPointer: ModuleOutputPointer, withName: String)
    fun addUserInput(patchModulePointer: ModulePointer, userInputPointer: UserInputPointer, withName: String)


    fun addPatch(patchModulePointer: ModulePointer, fromOutputPointer: ModuleOutputPointer, toInputPointer: ModuleInputPointer)
    fun resetPatch(patchModulePointer: ModulePointer)
}