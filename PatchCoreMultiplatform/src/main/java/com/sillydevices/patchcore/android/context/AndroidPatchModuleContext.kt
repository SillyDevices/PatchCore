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

package com.sillydevices.patchcore.android.context

import android.util.Log
import com.sillydevices.patchcore.android.jni.PatchModuleJni
import com.sillydevices.patchcore.android.jni.PolyModuleJni
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput

open class AndroidPatchModuleContext(
    pointer: ModulePointer,
    //TODO refactor to not use moduleFactory outside of the context
    val moduleFactory: ModuleFactory,
    contextFactory: ContextFactory,
): PatchModuleContext, AndroidModuleContext(pointer, contextFactory) {

    override fun createModule(module: FactoryModule): ModulePointer {
        val pointer = PatchModuleJni.patchModuleCreateModule(
            getPointer().nativePointer,
            module.moduleType.value,
            module.name,
            module.parameters)
        return ModulePointer(pointer)
    }

    override fun createPatchModule(newPatchModule: PatchModule): ModulePointer {
        val pointer = PatchModuleJni.patchModuleNew(
            moduleFactory.pointer.nativePointer,
            newPatchModule.name,
            newPatchModule.sampleRate)
        return ModulePointer(pointer)
    }

    override fun createPolyModule(newPolyModule: PolyModule): ModulePointer {
        val pointer = PolyModuleJni.polyModuleNew(
            moduleFactory.pointer.nativePointer,
            newPolyModule.name,
            newPolyModule.sampleRate,
            newPolyModule.polyphonyCount
        )
        return ModulePointer(pointer)
    }

    override fun addInput(input: ModuleInput, withName: String) {
        PatchModuleJni.patchModuleAddInput(
            getPointer().nativePointer,
            input.pointer.nativePointer,
            withName)
    }

    override fun addOutput(output: ModuleOutput, withName: String) {
        PatchModuleJni.patchModuleAddOutput(
            getPointer().nativePointer,
            output.pointer.nativePointer,
            withName)
    }

    override fun addUserInput(userInput: UserInput, withName: String) {
        PatchModuleJni.patchModuleAddUserInput(
            getPointer().nativePointer,
            userInput.pointer.nativePointer,
            withName)
    }

    override fun addPatch(output: ModuleOutput, input: ModuleInput) {
        Log.d("PatchModuleContext", "Adding patch from ${output.moduleName}:${output.name} to ${input.moduleName}${input.name}")
        PatchModuleJni.patchModuleAddPatch(
            getPointer().nativePointer,
            output.pointer.nativePointer,
            input.pointer.nativePointer)
    }

    override fun resetPatch() {
        PatchModuleJni.patchModuleResetPatch(getPointer().nativePointer)
    }

    override fun release() {
        PatchModuleJni.patchModuleRelease(getPointer().nativePointer)
    }

}