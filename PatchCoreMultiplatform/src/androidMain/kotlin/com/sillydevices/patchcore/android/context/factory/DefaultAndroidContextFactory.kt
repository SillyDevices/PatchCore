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

import com.sillydevices.patchcore.android.context.AndroidModuleContext
import com.sillydevices.patchcore.android.context.AndroidPatchCoreContext
import com.sillydevices.patchcore.android.context.AndroidPatchModuleContext
import com.sillydevices.patchcore.android.context.AndroidUserInputContext
import com.sillydevices.patchcore.android.context.modules.AndroidIndicatorModuleContext
import com.sillydevices.patchcore.android.context.modules.AndroidKeyboardModuleContext
import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchCoreContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.UserInputContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.context.factory.ModuleContextFactory
import com.sillydevices.patchcore.context.modules.IndicatorModuleContext
import com.sillydevices.patchcore.context.modules.KeyboardModuleContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.module.factory.ModuleFactory
import kotlin.reflect.KClass

internal class DefaultAndroidContextFactory(
    private val moduleContextFactory: ModuleContextFactory
): ContextFactory {

    override fun createPatchCoreContext(): PatchCoreContext {
        return AndroidPatchCoreContext(this)
    }

    override fun createPolyModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): PatchModuleContext {
        return createPatchModuleContext(pointer, parentContext)
    }

    override fun createPatchModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): PatchModuleContext {
        parentContext as AndroidPatchModuleContext
        return AndroidPatchModuleContext(pointer, parentContext.moduleFactory, parentContext.getContextFactory())
    }

    override fun createModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): ModuleContext {
        return AndroidModuleContext(pointer, this)
    }

    override fun createUserInputContext(pointer: UserInputPointer): UserInputContext {
        return AndroidUserInputContext(pointer)
    }

    override fun <T : ModuleContext> createModuleContext(
        moduleClass: KClass<T>,
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): T {
        return moduleContextFactory.createModuleContext(moduleClass, pointer, parentContext, this)
            ?: throw RuntimeException("Module context for ${moduleClass.simpleName} not found in factory: $moduleContextFactory")
    }
}