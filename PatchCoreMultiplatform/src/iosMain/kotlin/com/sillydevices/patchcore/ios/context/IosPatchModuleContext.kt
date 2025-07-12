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
import com.sillydevices.patchcore.module.factory.ModuleParameter.ParameterType
import kotlinx.cinterop.ExperimentalForeignApi

@OptIn(ExperimentalForeignApi::class)
open class IosPatchModuleContext(
    pointer: ModulePointer,
    //TODO refactor to not use moduleFactory outside of the context
    val moduleFactory: ModuleFactory,
    contextFactory: ContextFactory,
): PatchModuleContext, IosModuleContext(pointer, contextFactory) {

    override fun createModule(module: FactoryModule): ModulePointer {
        val parameters: Map<Any?, Any> = module.parameters.associate {
            val type = it.getType()
            val name = it.getName()
            name to when (type) {
                ParameterType.ENUM.ordinal -> ModuleParameterWrapper(it.getEnum())
                ParameterType.FLOAT.ordinal -> ModuleParameterWrapper(it.getFloat())
                ParameterType.BOOLEAN.ordinal -> ModuleParameterWrapper(it.getBoolean())
                else -> throw IllegalArgumentException("Unknown parameter type: $type")
            }
        }
        val pointer = patchModuleCreateModule(
            getPointer().nativePointer,
            module.moduleType.value,
            module.name,
            parameters)
        return ModulePointer(pointer)
    }

    override fun createPatchModule(newPatchModule: PatchModule): ModulePointer {
        val pointer = patchModuleNew(
            moduleFactory.pointer.nativePointer,
            newPatchModule.name,
            newPatchModule.sampleRate)
        return ModulePointer(pointer)
    }

    override fun createPolyModule(newPolyModule: PolyModule): ModulePointer {
        val pointer = polyModuleNew(
            moduleFactory.pointer.nativePointer,
            newPolyModule.name,
            newPolyModule.sampleRate,
            newPolyModule.polyphonyCount
        )
        return ModulePointer(pointer)
    }

    override fun addInput(input: ModuleInput, withName: String) {
        patchModuleAddInput(
            getPointer().nativePointer,
            input.pointer.nativePointer,
            withName)
    }

    override fun addOutput(output: ModuleOutput, withName: String) {
        patchModuleAddOutput(
            getPointer().nativePointer,
            output.pointer.nativePointer,
            withName)
    }

    override fun addUserInput(userInput: UserInput, withName: String) {
        patchModuleAddUserInput(
            getPointer().nativePointer,
            userInput.pointer.nativePointer,
            withName)
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