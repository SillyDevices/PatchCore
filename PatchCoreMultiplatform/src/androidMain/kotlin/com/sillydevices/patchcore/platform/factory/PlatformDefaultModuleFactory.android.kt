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

package com.sillydevices.patchcore.platform.factory

import com.sillydevices.patchcore.android.jni.modules.factory.DefaultModuleFactoryJni
import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.internal.pointers.WaveTableProviderPointer


actual object PlatformDefaultModuleFactory {
    actual fun new(waveTableProviderPointer: WaveTableProviderPointer, customModuleFactoryPointer: ModuleFactoryPointer?): ModuleFactoryPointer {
        return ModuleFactoryPointer(
            DefaultModuleFactoryJni.defaultModuleFactoryNew(
                waveTableProviderPointer.nativePointer,
                customModuleFactoryPointer?.nativePointer ?: ModuleFactoryPointer.NULL_VALUE
            ))
    }
}