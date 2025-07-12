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

package com.sillydevices.patchcore.modules

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.modules.IndicatorModuleContext
import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.factory.ModuleType

class IndicatorModule(name: String = "indicator"):
    FactoryModule(name, ModuleType.Indicator)
{
    val input = registerInput("in")

    fun setSize(newSize: Int) {
        (moduleContext as IndicatorModuleContext).setSize(newSize)
    }

    suspend fun copyInto(buffer: FloatArray, desiredCount: Int, startIndex: Int): Int {
        return (moduleContext as IndicatorModuleContext)
            .copyInto(buffer, desiredCount, startIndex)
    }

    override fun createFrom(parentContext: PatchModuleContext) {
        val pointer = parentContext.createModule(this)
        val newContext = parentContext.getContextFactory().createModuleContext(
            IndicatorModuleContext::class, pointer, parentContext
        )
        applyContext(newContext)
    }

    override fun applyContext(context: ModuleContext) {
        context as? IndicatorModuleContext ?: throw IllegalArgumentException("Context must be of type IndicatorModuleContext")
        super.applyContext(context)
    }
}