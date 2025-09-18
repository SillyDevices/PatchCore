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

package com.sillydevices.patchcore.platform.module

import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.internal.pointers.ModuleOutputPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.ios.wrappers.moduleGetModuleInput
import com.sillydevices.patchcore.ios.wrappers.moduleGetModuleOutput
import com.sillydevices.patchcore.ios.wrappers.moduleGetUserInput
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.cstr


@OptIn(ExperimentalForeignApi::class)
actual object PlatformModule {

    actual fun getModuleOutput(modulePointer: ModulePointer, outputName: String): ModuleOutputPointer {
        return ModuleOutputPointer(
            moduleGetModuleOutput(modulePointer.nativePointer, outputName.cstr)
        )
    }

    actual fun getModuleInput(modulePointer: ModulePointer, inputName: String): ModuleInputPointer {
        return ModuleInputPointer(
            moduleGetModuleInput(modulePointer.nativePointer, inputName.cstr)
        )
    }

    actual fun getUserInput(modulePointer: ModulePointer, userInputName: String): UserInputPointer {
        return UserInputPointer(
            moduleGetUserInput(modulePointer.nativePointer, userInputName.cstr)
        )
    }
}