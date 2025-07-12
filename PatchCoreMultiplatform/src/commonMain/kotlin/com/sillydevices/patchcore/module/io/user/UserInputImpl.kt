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

package com.sillydevices.patchcore.module.io.user

import com.sillydevices.patchcore.context.EmptyUserInputContext
import com.sillydevices.patchcore.context.UserInputContext
import com.sillydevices.patchcore.internal.NativeContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer

abstract class UserInputImpl(
    override val moduleName: String,
    override val name: String
) : UserInput {

    protected var context: UserInputContext = EmptyUserInputContext
        private set

    override fun applyParentContext(pointer: UserInputPointer, parentContext: NativeContext<ModulePointer>) {
        if (context !is EmptyUserInputContext) {
            throw IllegalStateException("UserInput context is already set. Cannot apply parent context again.")
        }
        context = parentContext.getContextFactory().createUserInputContext(pointer)
    }

    override val pointer: UserInputPointer
        get() = context.getPointer()
}