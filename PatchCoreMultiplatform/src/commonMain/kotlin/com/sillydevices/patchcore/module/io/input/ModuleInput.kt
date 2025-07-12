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

package com.sillydevices.patchcore.module.io.input

import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.module.io.NamedIO

open class ModuleInput(
    override val moduleName: String,
    override val name: String
): NamedIO {

    private var _pointer: ModuleInputPointer? = null

    val pointer: ModuleInputPointer
        get() = _pointer!!

    fun setPointer(pointer: ModuleInputPointer) {
        if (_pointer != null) {
            throw IllegalStateException("ModuleInput pointer is already set. Cannot set it again.")
        }
        _pointer = pointer
    }

    open fun createProxy(moduleName: String, withName: String): ProxyModuleInput {
        return ProxyModuleInput(moduleName, withName, this)
    }

    override fun equals(other: Any?): Boolean {
        return if (other is ModuleInput){
            moduleName == other.moduleName && name == other.name
        } else super.equals(other)
    }

    override fun hashCode(): Int = arrayOf("input", moduleName, name).contentHashCode()
}