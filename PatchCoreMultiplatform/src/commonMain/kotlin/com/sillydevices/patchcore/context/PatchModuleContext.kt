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

package com.sillydevices.patchcore.context

import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import com.sillydevices.patchcore.platform.module.PlatformPatchModule
import com.sillydevices.patchcore.platform.module.PlatformPolyModule

interface PatchModuleContext: ModuleContext {
    val exposedIoAlreadyExists: Boolean

    fun createModule(module: FactoryModule): ModulePointer
    fun createPatchModule(newPatchModule: PatchModule): ModulePointer
    fun createPolyModule(newPolyModule: PolyModule): ModulePointer

    fun addModule(pointer: ModulePointer, name: String): ModulePointer
    fun getModule(name: String): ModulePointer

    fun exposeInput(input: ModuleInput, withName: String)
    fun exposeOutput(output: ModuleOutput, withName: String)
    fun exposeUserInput(userInput: UserInput, withName: String)


    fun resetPatch()
    fun addPatch(output: ModuleOutput, input: ModuleInput)

    fun removePatch(output: ModuleOutput, input: ModuleInput)


    fun release()
}


open class PatchModuleContextImpl(
    pointer: ModulePointer,
    //TODO refactor to not use moduleFactory outside of the context
    val moduleFactory: ModuleFactory,
    contextFactory: ContextFactory,
    override val exposedIoAlreadyExists: Boolean = false,
): PatchModuleContext, ModuleContextImpl(pointer, contextFactory) {

    override fun createModule(module: FactoryModule): ModulePointer {
        return PlatformPatchModule.createModule(
            getPointer(),
            module.moduleType.value,
            module.name,
            module.parameters)
    }

    //TODO rename, now it is confusing(create new patchmodule within current patchmodule or create new patchmodule)
    override fun createPatchModule(newPatchModule: PatchModule): ModulePointer {
        return PlatformPatchModule.new(
            moduleFactory.pointer,
            newPatchModule.name,
            newPatchModule.sampleRate)
    }

    override fun createPolyModule(newPolyModule: PolyModule): ModulePointer {
       return PlatformPolyModule.new(
            moduleFactory.pointer,
            newPolyModule.name,
            newPolyModule.sampleRate,
            newPolyModule.polyphonyCount
       )
    }

    override fun addModule(pointer: ModulePointer, name: String): ModulePointer {
        return PlatformPatchModule.addModule(getPointer(), pointer)
    }

    override fun getModule(name: String): ModulePointer {
        return PlatformPatchModule.getModule(getPointer(), name)
    }


    override fun exposeInput(input: ModuleInput, withName: String) {
        PlatformPatchModule.exposeInput(
            getPointer(),
            input.pointer,
            withName)
    }

    override fun exposeOutput(output: ModuleOutput, withName: String) {
        PlatformPatchModule.exposeOutput(
            getPointer(),
            output.pointer,
            withName)
    }

    override fun exposeUserInput(userInput: UserInput, withName: String) {
        PlatformPatchModule.exposeUserInput(
            getPointer(),
            userInput.pointer,
            withName)
    }

    override fun resetPatch() {
        PlatformPatchModule.resetPatch(getPointer())
    }

    override fun addPatch(output: ModuleOutput, input: ModuleInput) {
        PlatformPatchModule.addPatch(
            getPointer(),
            output.pointer,
            input.pointer)
    }

    override fun removePatch(output: ModuleOutput, input: ModuleInput) {
        PlatformPatchModule.removePatch(
            getPointer(),
            output.pointer,
            input.pointer)
    }



    //TODO move release to parent class
    override fun release() {
        PlatformPatchModule.delete(getPointer())
    }

}
