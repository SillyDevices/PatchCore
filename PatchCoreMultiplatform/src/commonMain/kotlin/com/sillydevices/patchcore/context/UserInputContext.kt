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
import com.sillydevices.patchcore.internal.NativeContextWithPointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.platform.module.PlatformUserInput

interface UserInputContext : NativeContextWithPointer<UserInputPointer> {

    fun setFloatUserInputValue(value: Float)
    fun setBooleanUserInputValue(value: Boolean)
    fun setEnumUserInputValue(value: Int)
}


class UserInputContextImpl(
    private val userInputPointer: UserInputPointer,
) : UserInputContext {

    override fun setFloatUserInputValue(value: Float) {
        PlatformUserInput.setValue(userInputPointer, value)
    }

    override fun setBooleanUserInputValue(value: Boolean) {
        PlatformUserInput.setValue(userInputPointer, value)
    }

    override fun setEnumUserInputValue(value: Int) {
        PlatformUserInput.setValue(userInputPointer, value)
    }

    override fun getPointer(): UserInputPointer = userInputPointer


    override fun getContextFactory(): ContextFactory {
        throw UnsupportedOperationException("AndroidUserInputContext does not support context factory")
    }
}

object EmptyUserInputContext : UserInputContext {

    override fun getPointer(): UserInputPointer {
        throw UnsupportedOperationException("EmptyUserInputContext does not have a pointer")
    }

    override fun getContextFactory() =
        throw UnsupportedOperationException("EmptyUserInputContext does not support context factory")

    override fun setFloatUserInputValue(value: Float) {
        throw UnsupportedOperationException("EmptyUserInputContext does not support float user input values")
    }

    override fun setBooleanUserInputValue(value: Boolean) {
        throw UnsupportedOperationException("EmptyUserInputContext does not support boolean user input values")
    }

    override fun setEnumUserInputValue(value: Int) {
        throw UnsupportedOperationException("EmptyUserInputContext does not support enum user input values")
    }
}