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

import com.sillydevices.patchcore.context.UserInputContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.ios.wrappers.*
import kotlinx.cinterop.ExperimentalForeignApi

@OptIn(ExperimentalForeignApi::class)
class IosUserInputContext(
      private val userInputPointer: UserInputPointer,
): UserInputContext {

    override fun setFloatUserInputValue(value: Float) {
        floatUserInputSetValue(userInputPointer.nativePointer, value)
    }

    override fun setBooleanUserInputValue(value: Boolean) {
        boolUserInputSetValue(userInputPointer.nativePointer, value)
    }

    override fun setEnumUserInputValue(value: Int) {
        enumUserInputSetValue(userInputPointer.nativePointer, value)
    }

    override fun getPointer(): UserInputPointer = userInputPointer


    override fun getContextFactory(): ContextFactory {
        throw UnsupportedOperationException("AndroidUserInputContext does not support context factory")
    }

}

