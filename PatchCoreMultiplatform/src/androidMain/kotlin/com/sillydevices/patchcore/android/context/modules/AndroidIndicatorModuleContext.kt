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

package com.sillydevices.patchcore.android.context.modules

import com.sillydevices.patchcore.android.context.AndroidModuleContext
import com.sillydevices.patchcore.android.jni.modules.IndicatorModuleJni
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.context.modules.IndicatorModuleContext
import com.sillydevices.patchcore.internal.pointers.ModulePointer

class AndroidIndicatorModuleContext(pointer: ModulePointer, contextFactory: ContextFactory):
    IndicatorModuleContext,
    AndroidModuleContext(pointer, contextFactory)
{
    override fun setSize(newSize: Int) {
        IndicatorModuleJni.setIndicatorBufferSize(getPointer().nativePointer, newSize)
    }

    override suspend fun copyInto(buffer: FloatArray, desiredCount: Int, startIndex: Int): Int {
        return IndicatorModuleJni.copyIndicatorBuffer(
            getPointer().nativePointer,
            buffer,
            desiredCount,
            startIndex
        )
    }

}