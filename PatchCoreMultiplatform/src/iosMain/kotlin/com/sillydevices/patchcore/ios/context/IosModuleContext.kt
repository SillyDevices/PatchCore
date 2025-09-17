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

package com.sillydevices.patchcore.ios.context

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.internal.pointers.ModuleOutputPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import com.sillydevices.patchcore.ios.wrappers.*
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.cstr


@OptIn(ExperimentalForeignApi::class)
open class IosModuleContext(
    pointer: ModulePointer,
    contextFactory: ContextFactory,
): ModuleContext {

    protected val _contextFactory: ContextFactory = contextFactory
    protected val _pointer: ModulePointer = pointer

    override fun getPointer(): ModulePointer {
        return _pointer
    }

    override fun getModuleInputPointer(input: ModuleInput): ModuleInputPointer {
        val inputPointer = moduleGetModuleInput(_pointer.nativePointer, input.name.cstr)
        return ModuleInputPointer(inputPointer)
    }

    override fun getModuleOutputPointer(output: ModuleOutput): ModuleOutputPointer {
        val outputPointer = moduleGetModuleOutput(_pointer.nativePointer, output.name.cstr)
        return ModuleOutputPointer(outputPointer)
    }

    override fun getUserInputPointer(userInput: UserInput): UserInputPointer {
        val userInputPointer = moduleGetUserInput(_pointer.nativePointer, userInput.name.cstr)
        return UserInputPointer(userInputPointer)
    }


    override fun getContextFactory(): ContextFactory {
        return _contextFactory
    }

}