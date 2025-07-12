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

package com.sillydevices.patchcore.modules.input

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.modules.KeyboardModuleContext
import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.factory.ModuleType

abstract class AbstractKeyboardModule(name: String, moduleType: ModuleType): FactoryModule(name, moduleType) {

    val gate = registerOutput("gate")
    val cv = registerOutput("cv")

    val userRetrigger = registerBooleanUserInput("mt")

    fun onPress(cvValue: Float, velocity: Float = 1f) {
        (moduleContext as KeyboardModuleContext).onPress(cvValue, velocity)
    }

    fun onRelease(cvValue: Float) {
        (moduleContext as KeyboardModuleContext).onRelease(cvValue)
    }

    override fun createFrom(parentContext: PatchModuleContext) {
        val pointer = parentContext.createModule(this)
        val newContext = parentContext.getContextFactory().createModuleContext(
            KeyboardModuleContext::class,
            pointer,
            parentContext
        )
        applyContext(newContext)
    }

    override fun applyContext(context: ModuleContext) {
        context as? KeyboardModuleContext ?: throw IllegalArgumentException("Context must be of type KeyboardModuleContext")
        super.applyContext(context)
    }


}