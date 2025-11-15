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

package com.sillydevices.patchcore.platform.modules

import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.types.IndicatorBuffer
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.refTo
import com.sillydevices.patchcore.ios.wrappers.setIndicatorBufferSize as iOSSetIndicatorBufferSize
import com.sillydevices.patchcore.ios.wrappers.copyIndicatorBuffer as iOSCopyIndicatorBuffer

@OptIn(ExperimentalForeignApi::class)
actual object PlatformIndicatorModule {

    actual fun getDirectIndicatorBuffer(modulePointer: ModulePointer, timeScale: Float): IndicatorBuffer {
        throw NotImplementedError("Direct Indicator Buffer access is not implemented for iOS yet.")
    }

}