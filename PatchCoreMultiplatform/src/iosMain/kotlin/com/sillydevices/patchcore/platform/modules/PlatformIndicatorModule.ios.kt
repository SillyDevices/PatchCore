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
import com.sillydevices.patchcore.ios.wrappers.NativeBuffer
import com.sillydevices.patchcore.types.IndicatorBuffer
import kotlinx.cinterop.CPointer
import kotlinx.cinterop.CValue
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.FloatVar
import kotlinx.cinterop.IntVar
import kotlinx.cinterop.get
import kotlinx.cinterop.reinterpret
import kotlinx.cinterop.useContents
import com.sillydevices.patchcore.ios.wrappers.getDirectIndicatorBuffer as iOSGetDirectIndicatorBuffer

@OptIn(ExperimentalForeignApi::class)
actual object PlatformIndicatorModule {


    class IndicatorBufferImpl(
        private val nativeBuffer: CPointer<FloatVar>,
        size: Int
    ): IndicatorBuffer {

        class ListBufferWrapper(private val floatBuffer: CPointer<FloatVar>, override val size: Int, val dataOffset: Int = 0): List<Float> {


            override fun isEmpty(): Boolean {
                return size == 0
            }

            override fun contains(element: Float): Boolean {
                TODO("Not yet implemented")
            }

            override fun iterator(): Iterator<Float> {
                TODO("Not yet implemented")
            }

            override fun containsAll(elements: Collection<Float>): Boolean {
                TODO("Not yet implemented")
            }

            override fun get(index: Int): Float {
                val result = floatBuffer.get(index + dataOffset)
                return result
            }

            override fun indexOf(element: Float): Int {
                TODO("Not yet implemented")
            }

            override fun lastIndexOf(element: Float): Int {
                TODO("Not yet implemented")
            }

            override fun listIterator(): ListIterator<Float> {
                TODO("Not yet implemented")
            }

            override fun listIterator(index: Int): ListIterator<Float> {
                TODO("Not yet implemented")
            }

            override fun subList(
                fromIndex: Int,
                toIndex: Int
            ): List<Float> {
                TODO("Not yet implemented")
            }

        }
        private val floatList = ListBufferWrapper(nativeBuffer, size, 4)

        private val indexBuffer: CPointer<IntVar> = nativeBuffer.reinterpret()

        override fun getList(): List<Float> = floatList

        override fun getWriteIndex(): Int {
            val result = indexBuffer[0]
            return result
        }

    }

    actual fun getDirectIndicatorBuffer(modulePointer: ModulePointer, timeScale: Float): IndicatorBuffer {
        val buffer: CValue<NativeBuffer> = iOSGetDirectIndicatorBuffer(modulePointer.nativePointer, timeScale)
        val (size, dataPtr) = buffer.useContents { size to data }
        dataPtr ?: throw RuntimeException("Failed to get data pointer from NativeBuffer.")
        return IndicatorBufferImpl(dataPtr, size)
    }

}