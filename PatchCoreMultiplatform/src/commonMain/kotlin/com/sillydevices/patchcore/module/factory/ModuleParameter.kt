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

package com.sillydevices.patchcore.module.factory

class ModuleParameter {
    constructor(name: String, value: Float) {
        this.name = name
        floatValue = value
        enumValue = 0
        boolValue = false
        type = ParameterType.FLOAT.ordinal

    }
    constructor(name: String, value: Int) {
        this.name = name
        floatValue = 0f
        enumValue = value
        boolValue = false
        type = ParameterType.ENUM.ordinal
    }
    constructor(name: String, value: Boolean) {
        this.name = name
        floatValue = 0f
        enumValue = 0
        boolValue = value
        type = ParameterType.BOOLEAN.ordinal
    }

    enum class ParameterType {
         ENUM, FLOAT, BOOLEAN
    }
    private val name: String
    private val floatValue: Float
    private val enumValue: Int
    private val boolValue: Boolean
    private val type: Int

    fun getName(): String = name
    fun getType(): Int = type
    fun getFloat(): Float = floatValue
    fun getEnum(): Int = enumValue
    fun getBoolean(): Boolean = boolValue
}