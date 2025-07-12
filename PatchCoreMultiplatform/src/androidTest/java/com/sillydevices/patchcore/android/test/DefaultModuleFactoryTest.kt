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

package com.sillydevices.patchcore.android.test

import androidx.test.ext.junit.runners.AndroidJUnit4
import com.sillydevices.patchcore.PatchCore
import com.sillydevices.patchcore.android.context.AndroidPatchCoreContext
import com.sillydevices.patchcore.android.context.factory.DefaultAndroidContextFactory
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class DefaultModuleFactoryTest {
    @Test
    fun testDefaultModuleFactoryTestAndRelease() {
        val patchCore = PatchCore()
        patchCore.release()
    }
}