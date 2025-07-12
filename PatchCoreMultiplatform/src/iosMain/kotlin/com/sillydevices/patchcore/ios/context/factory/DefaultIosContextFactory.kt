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

package com.sillydevices.patchcore.ios.context.factory


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
import com.sillydevices.patchcore.ios.context.IosModuleContext
import com.sillydevices.patchcore.ios.context.IosPatchCoreContext
import com.sillydevices.patchcore.ios.context.IosPatchModuleContext
import com.sillydevices.patchcore.ios.context.IosUserInputContext
import com.sillydevices.patchcore.ios.context.modules.IosIndicatorModuleContext
import com.sillydevices.patchcore.ios.context.modules.IosKeyboardModuleContext
import kotlinx.cinterop.ExperimentalForeignApi
import kotlin.reflect.KClass


@OptIn(ExperimentalForeignApi::class)
open class DefaultIosContextFactory(
    private val moduleContextFactory: ModuleContextFactory
): ContextFactory {

    override fun createPatchCoreContext(): PatchCoreContext {
        return IosPatchCoreContext(this)
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
        parentContext as IosPatchModuleContext
        return IosPatchModuleContext(pointer, parentContext.moduleFactory, parentContext.getContextFactory())
    }

    override fun createModuleContext(
        pointer: ModulePointer,
        parentContext: PatchModuleContext
    ): ModuleContext {
        return IosModuleContext(pointer, this)
    }

    override fun createUserInputContext(pointer: UserInputPointer): UserInputContext {
        return IosUserInputContext(pointer)
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