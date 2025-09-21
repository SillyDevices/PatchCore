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
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.platform.module.PlatformPolyModule

interface PolyModuleContext: PatchModuleContext {
    fun setActiveVoicesCount(count: Int)
    fun getActiveVoicesCount(): Int
}

open class PolyModuleContextImpl(
    pointer: ModulePointer,
    moduleFactory: ModuleFactory,
    contextFactory: ContextFactory,
): PatchModuleContextImpl(pointer, moduleFactory, contextFactory), PolyModuleContext {

    override fun setActiveVoicesCount(count: Int) {
        PlatformPolyModule.setActiveVoicesCount(getPointer(), count)
    }

    override fun getActiveVoicesCount(): Int {
        return PlatformPolyModule.getActiveVoicesCount(getPointer())
    }

}