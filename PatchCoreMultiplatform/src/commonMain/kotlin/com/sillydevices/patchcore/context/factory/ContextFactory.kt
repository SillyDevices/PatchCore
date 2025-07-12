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

package com.sillydevices.patchcore.context.factory

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchCoreContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.UserInputContext
import com.sillydevices.patchcore.context.modules.IndicatorModuleContext
import com.sillydevices.patchcore.context.modules.KeyboardModuleContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.module.Module
import com.sillydevices.patchcore.module.factory.ModuleFactory
import kotlin.reflect.KClass

interface ContextFactory {

    fun createPatchCoreContext(): PatchCoreContext

    fun createPolyModuleContext(pointer: ModulePointer, parentContext: PatchModuleContext): PatchModuleContext
    fun createPatchModuleContext(pointer: ModulePointer, parentContext: PatchModuleContext) : PatchModuleContext
    fun createModuleContext(pointer: ModulePointer, parentContext: PatchModuleContext): ModuleContext

    fun createUserInputContext(pointer: UserInputPointer): UserInputContext

    fun <T : ModuleContext> createModuleContext(
        moduleClass: KClass<T>,
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): T

//    fun createKeyboardModuleContext(pointer: ModulePointer, parentContext: PatchModuleContext): KeyboardModuleContext
//    fun createIndicatorModuleContext(pointer: ModulePointer, parentContext: PatchModuleContext): IndicatorModuleContext
}