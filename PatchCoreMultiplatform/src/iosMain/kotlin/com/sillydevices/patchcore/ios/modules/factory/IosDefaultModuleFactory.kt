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

package com.sillydevices.patchcore.ios.modules.factory

import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.ios.wrappers.DefaultModuleFactoryWrapper
import com.sillydevices.patchcore.module.factory.DefaultModuleFactory
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.toLong

@OptIn(ExperimentalForeignApi::class)
class IosDefaultModuleFactory(wrapper: DefaultModuleFactoryWrapper) : IosModuleFactory(), DefaultModuleFactory {

    //TODO move to IosModuleFactory
    private val factory = wrapper
    private val _pointer = ModuleFactoryPointer(wrapper.getRawPointerToModuleFactory().toLong().toULong())
    override val pointer = _pointer
}

