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

package com.sillydevices.patchcore.module

import com.sillydevices.patchcore.context.EmptyModuleContext
import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput

abstract class Module(val name: String) {

    protected val registeredInputs = mutableListOf<ModuleInput>()
    protected val registeredOutputs = mutableListOf<ModuleOutput>()
    protected val registeredUserInputs = mutableListOf<UserInput>()

    protected fun registerInput(name: String): ModuleInput {
        val input = ModuleInput(this.name, name)
        registerInput(input)
        return input
    }

    private fun <T:ModuleInput> registerInput(input: T): T {
        registeredInputs.add(input)
        return input
    }

    protected fun registerOutput(name: String): ModuleOutput {
        val output = ModuleOutput(this.name, name)
        registerOutput(output)
        return output
    }

    private fun <T:ModuleOutput> registerOutput(output: T): T {
        registeredOutputs.add(output)
        return output
    }

    protected fun <T: UserInput> registerUserInput(userInput: T): T {
        registeredUserInputs.add(userInput)
        return userInput
    }

    protected var moduleContext: ModuleContext = EmptyModuleContext
        private set

    open fun applyContext(context: ModuleContext) {
        if (moduleContext !is EmptyModuleContext) {
            throw IllegalStateException("Module context is already set. Cannot apply context again.")
        }
        this.moduleContext = context
    }

    //Context and so on
    abstract fun createFrom(context: PatchModuleContext)

    open val pointer: ModulePointer
        get() = moduleContext.getPointer()
}











