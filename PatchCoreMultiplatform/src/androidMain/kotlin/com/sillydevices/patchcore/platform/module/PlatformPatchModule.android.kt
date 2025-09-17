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

import com.sillydevices.patchcore.android.jni.PatchModuleJni
import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.internal.pointers.ModuleOutputPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.module.factory.ModuleParameter

actual object PlatformPatchModule {

    actual fun new(moduleFactoryPointer: ModuleFactoryPointer, name: String, sampleRate: Int): ModulePointer {
        return ModulePointer(
            PatchModuleJni.patchModuleNew(
                moduleFactoryPointer.nativePointer,
                name,
                sampleRate
            )
        )
    }

    actual fun release(patchModulePointer: ModulePointer) {
        PatchModuleJni.patchModuleRelease(patchModulePointer.nativePointer)
    }

    actual fun createModule(patchModulePointer: ModulePointer, moduleType: String, moduleName: String, parameters: List<ModuleParameter>): ModulePointer {
        return ModulePointer(
            PatchModuleJni.patchModuleCreateModule(
                patchModulePointer.nativePointer,
                moduleType,
                moduleName,
                parameters
            )
        )
    }

    actual fun addInput(patchModulePointer: ModulePointer, inputPointer: ModuleInputPointer, withName: String) {
        PatchModuleJni.patchModuleAddInput(
            patchModulePointer.nativePointer,
            inputPointer.nativePointer,
            withName
        )
    }

    actual fun addOutput(patchModulePointer: ModulePointer, outputPointer: ModuleOutputPointer, withName: String) {
        PatchModuleJni.patchModuleAddOutput(
            patchModulePointer.nativePointer,
            outputPointer.nativePointer,
            withName
        )
    }

    actual fun addUserInput(patchModulePointer: ModulePointer, userInputPointer: UserInputPointer, withName: String) {
        PatchModuleJni.patchModuleAddUserInput(
            patchModulePointer.nativePointer,
            userInputPointer.nativePointer,
            withName
        )
    }

    actual fun addPatch(patchModulePointer: ModulePointer, fromOutputPointer: ModuleOutputPointer, toInputPointer: ModuleInputPointer) {
        PatchModuleJni.patchModuleAddPatch(
            patchModulePointer.nativePointer,
            fromOutputPointer.nativePointer,
            toInputPointer.nativePointer
        )
    }

    actual fun resetPatch(patchModulePointer: ModulePointer) {
        PatchModuleJni.patchModuleResetPatch(patchModulePointer.nativePointer)
    }
}