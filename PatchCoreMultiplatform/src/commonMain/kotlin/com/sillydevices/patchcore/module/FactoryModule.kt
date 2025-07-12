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

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.module.factory.ModuleParameter
import com.sillydevices.patchcore.module.factory.ModuleType
import com.sillydevices.patchcore.module.io.user.BooleanUserInput
import com.sillydevices.patchcore.module.io.user.EnumUserInput
import com.sillydevices.patchcore.module.io.user.FloatUserInput

abstract class FactoryModule(name: String, val moduleType: ModuleType): Module(name) {
    open val parameters: List<ModuleParameter> = listOf()

    protected fun registerFloatUserInput(name: String, range: ClosedRange<Float>): FloatUserInput {
        val input = FloatUserInput(this.name, name, range)
        registerUserInput(input)
        return input
    }
    protected fun registerBooleanUserInput(name: String): BooleanUserInput {
        val input = BooleanUserInput(this.name, name)
        registerUserInput(input)
        return input
    }
    protected fun <T : Enum<T>> registerEnumUserInput(name: String): EnumUserInput<T> {
        val input = EnumUserInput<T>(this.name, name)
        registerUserInput(input)
        return input
    }

    override fun createFrom(parentContext: PatchModuleContext) {
        val pointer = parentContext.createModule(this)
        val newContext = parentContext.getContextFactory().createModuleContext(pointer, parentContext)
        applyContext(newContext)
    }


    override fun applyContext(context: ModuleContext) {
        super.applyContext(context)
        for (input in registeredInputs) {
            val pointer = context.getModuleInputPointer(input)
            input.setPointer(pointer)
        }
        for (output in registeredOutputs) {
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }
        for (userInput in registeredUserInputs) {
            val pointer = context.getUserInputPointer(userInput)
            userInput.applyParentContext(pointer, context)
        }
    }

}