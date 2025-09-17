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

package com.sillydevices.patchcore.ios.context

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
import com.sillydevices.patchcore.ios.wrappers.*
import com.sillydevices.patchcore.module.factory.ModuleParameter
import kotlinx.cinterop.AutofreeScope
import kotlinx.cinterop.CArrayPointer
import kotlinx.cinterop.CPointer
import kotlinx.cinterop.CValuesRef
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.IntVar
import kotlinx.cinterop.MemScope
import kotlinx.cinterop.NativePointed
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.asStableRef
import kotlinx.cinterop.cValue
import kotlinx.cinterop.cValuesOf
import kotlinx.cinterop.cstr
import kotlinx.cinterop.get
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.nativeHeap

@OptIn(ExperimentalForeignApi::class)
open class IosPatchModuleContext(
    pointer: ModulePointer,
    //TODO refactor to not use moduleFactory outside of the context
    val moduleFactory: ModuleFactory,
    contextFactory: ContextFactory,
): PatchModuleContext, IosModuleContext(pointer, contextFactory) {

    override fun createModule(module: FactoryModule): ModulePointer {
        memScoped {
            val paramCount = module.parameters.size
            val paramsArray = allocArray<ModuleParameterWrapper>(paramCount)
            module.parameters.forEachIndexed { i, it ->
                paramsArray.get(i).apply {
                    type = it.getType()
                    enumValue = it.getEnum()
                    boolValue = if (it.getBoolean()) 1 else 0
                    floatValue = it.getFloat()
                    key = it.getName().cstr.getPointer(this@memScoped)
                }
            }
            val pointer = patchModuleCreateModule(
                getPointer().nativePointer,
                module.moduleType.value.cstr,
                module.name.cstr,
                paramsArray,
                paramCount.toULong()
            )
            return ModulePointer(pointer)
        }
    }

    override fun createPatchModule(newPatchModule: PatchModule): ModulePointer {
        val pointer = patchModuleNew(
            moduleFactory.pointer.nativePointer,
            newPatchModule.name.cstr,
            newPatchModule.sampleRate)
        return ModulePointer(pointer)
    }

    override fun createPolyModule(newPolyModule: PolyModule): ModulePointer {
        val pointer = polyModuleNew(
            moduleFactory.pointer.nativePointer,
            newPolyModule.name.cstr,
            newPolyModule.sampleRate,
            newPolyModule.polyphonyCount
        )
        return ModulePointer(pointer)
    }

    override fun addInput(input: ModuleInput, withName: String) {
        patchModuleAddInput(
            getPointer().nativePointer,
            input.pointer.nativePointer,
            withName.cstr)
    }

    override fun addOutput(output: ModuleOutput, withName: String) {
        patchModuleAddOutput(
            getPointer().nativePointer,
            output.pointer.nativePointer,
            withName.cstr)
    }

    override fun addUserInput(userInput: UserInput, withName: String) {
        patchModuleAddUserInput(
            getPointer().nativePointer,
            userInput.pointer.nativePointer,
            withName.cstr)
    }

    override fun addPatch(output: ModuleOutput, input: ModuleInput) {
        patchModuleAddPatch(
            getPointer().nativePointer,
            output.pointer.nativePointer,
            input.pointer.nativePointer)
    }

    override fun resetPatch() {
        patchModuleResetPatch(getPointer().nativePointer)
    }

    override fun release() {
        patchModuleRelease(getPointer().nativePointer)
    }

}