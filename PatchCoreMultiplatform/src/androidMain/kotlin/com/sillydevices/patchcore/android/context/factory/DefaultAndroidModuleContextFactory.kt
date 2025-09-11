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

package com.sillydevices.patchcore.android.context.factory

import com.sillydevices.patchcore.android.context.modules.AndroidIndicatorModuleContext
import com.sillydevices.patchcore.android.context.modules.AndroidKeyboardModuleContext
import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.context.factory.ModuleContextFactory
import com.sillydevices.patchcore.context.modules.IndicatorModuleContext
import com.sillydevices.patchcore.context.modules.KeyboardModuleContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import kotlin.reflect.KClass

internal class DefaultAndroidModuleContextFactory(val customModuleContextFactory: ModuleContextFactory?): ModuleContextFactory {

    override fun <T : ModuleContext> createModuleContext(
        moduleClass: KClass<T>,
        pointer: ModulePointer,
        parentContext: PatchModuleContext,
        contextFactory: ContextFactory
    ): T? {
        val moduleContext = customModuleContextFactory?.createModuleContext(moduleClass, pointer, parentContext, contextFactory)
        if (moduleContext != null) {
            return moduleContext as T
        }
        when (moduleClass) {
            KeyboardModuleContext::class -> {
                return createKeyboardModuleContext(pointer, parentContext, contextFactory) as T
            }
            IndicatorModuleContext::class -> {
                return createIndicatorModuleContext(pointer, parentContext, contextFactory) as T
            }
        }
        return null
    }

    private fun createKeyboardModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext,
        contextFactory: ContextFactory
    ): KeyboardModuleContext {
        return AndroidKeyboardModuleContext(pointer, contextFactory)
    }

    private fun createIndicatorModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext,
        contextFactory: ContextFactory
    ): IndicatorModuleContext {
        return AndroidIndicatorModuleContext(pointer, contextFactory)
    }
}