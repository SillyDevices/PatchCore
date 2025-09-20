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

import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.PolyModuleContext

open class PolyModule(name: String, val polyphonyCount: Int): PatchModule(name) {

    override fun createFrom(parentContext: PatchModuleContext) {
        val pointer = parentContext.createPolyModule(this)
        val newContext = parentContext.getContextFactory().createPolyModuleContext(pointer, parentContext)
        applyContext(newContext)
    }

    fun setActiveVoicesCount(count: Int) {
        if (count < 1 || count > polyphonyCount) {
            throw IllegalArgumentException("Active voices count must be in range 1..$polyphonyCount")
        }
        val context = moduleContext as? PolyModuleContext ?: throw IllegalStateException("Module context is not set")
        context.setActiveVoicesCount(count)
    }

    fun getActiveVoicesCount(): Int {
        val context = moduleContext as? PolyModuleContext ?: throw IllegalStateException("Module context is not set")
        return context.getActiveVoicesCount()
    }

}