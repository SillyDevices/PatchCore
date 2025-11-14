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

import com.sillydevices.patchcore.android.jni.modules.IndicatorModuleJni
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.modules.IndicatorModule
import com.sillydevices.patchcore.types.IndicatorBuffer
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

actual object PlatformIndicatorModule {

    class IndicatorBufferImpl(
        val jniBuffer: ByteBuffer
    ): IndicatorBuffer {

        class ListBufferWrapper(private val floatBuffer: FloatBuffer): List<Float> {
            override val size: Int = floatBuffer.limit()

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
                return floatBuffer.get(index)
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

        init {
            jniBuffer.order(ByteOrder.nativeOrder())
        }
        private val buffer = jniBuffer.asFloatBuffer()
        private val floatList = ListBufferWrapper(buffer)

        override fun getList(): List<Float> = floatList

    }

    actual fun getDirectIndicatorBuffer(modulePointer: ModulePointer, timeScale: Float): IndicatorBuffer {
        val buffer: ByteBuffer = IndicatorModuleJni.getDirectIndicatorBuffer(modulePointer.nativePointer, timeScale)
        return IndicatorBufferImpl(buffer)
    }

    actual fun setIndicatorBufferSize(modulePointer: ModulePointer, size: Int) {
        IndicatorModuleJni.setIndicatorBufferSize(modulePointer.nativePointer, size)
    }

    actual fun copyIndicatorBuffer(modulePointer: ModulePointer, buffer: FloatArray, size: Int, startIndex: Int): Int {
        return IndicatorModuleJni.copyIndicatorBuffer(modulePointer.nativePointer, buffer, size, startIndex)
    }


}