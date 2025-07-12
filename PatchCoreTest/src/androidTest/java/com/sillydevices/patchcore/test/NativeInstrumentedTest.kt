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

package com.sillydevices.patchcore.test

import androidx.test.ext.junit.runners.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class NativeInstrumentedTest {

    external fun runNativeTests(): Int
    external fun runSingleNativeTest(testName: String): Int

    var nativeLibraryLoaded = false
    fun loadLibrary() {
        if (nativeLibraryLoaded) return
        System.loadLibrary("patchcoretest")
        nativeLibraryLoaded = true
    }

    @Test
    fun nativeTests() {
        loadLibrary()
        if (runNativeTests() != 0) {
            throw RuntimeException("Native tests failed")
        }
//        // Context of the app under test.
//        val appContext = InstrumentationRegistry.getInstrumentation().targetContext
//        assertEquals("com.sillydevices.patchcore.test", appContext.packageName)
    }

    @Test
    fun runSingleNativeTest() {
        loadLibrary()
//        val testName = "PatchModuleTest.PatchModuleBasicTest"
//        val testName = "PatchModuleTest.interolatedUserInputTest"
        val testName = "PolyModuleTest.PatchModuleToPolyUserInputTest"
        if (runSingleNativeTest(testName) != 0) {
            throw RuntimeException("Native test '$testName' failed")
        }
    }

}