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

package com.sillydevices.patchcore.android.jni

import com.sillydevices.patchcore.module.factory.ModuleParameter

object PatchModuleJni {

    @JvmName("patchModuleNew")
    external fun patchModuleNew(moduleFactoryPointer: ULong, name: String, sampleRate: Int): ULong
    @JvmName("patchModuleRelease")
    external fun patchModuleRelease(patchModuleManagedPointer: ULong)

    @JvmName("patchModuleCreateModule")
    external fun patchModuleCreateModule(patchModuleManagedPointer: ULong, moduleType: String, moduleName: String, parameters: List<ModuleParameter>): ULong
//    external fun patchModuleAddModule(patchModuleManagedPointer: ULong, managedModulePointer: ULong)
//    external fun patchModuleGetModule(patchModulePointer: ULong, moduleName: String): ULong

    @JvmName("patchModuleAddInput")
    external fun patchModuleAddInput(patchModuleManagedPointer: ULong, inputPointer: ULong, inputName: String)
    @JvmName("patchModuleAddOutput")
    external fun patchModuleAddOutput(patchModuleManagedPointer: ULong, outputPointer: ULong, outputName: String)
    @JvmName("patchModuleAddUserInput")
    external fun patchModuleAddUserInput(patchModuleManagedPointer: ULong, userInputPointer: ULong, inputName: String)

    @JvmName("patchModuleAddPatch")
    external fun patchModuleAddPatch(patchModulePointer: ULong, fromOutputPointer: ULong, toInputPointer: ULong)
    @JvmName("patchModuleResetPatch")
    external fun patchModuleResetPatch(patchModulePointer: ULong)
}