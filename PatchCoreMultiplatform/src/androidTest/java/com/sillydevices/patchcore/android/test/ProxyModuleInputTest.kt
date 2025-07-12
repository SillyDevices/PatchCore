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
import com.sillydevices.patchcore.android.utils.SynthTester
import com.sillydevices.patchcore.android.utils.TestPatchCore
import com.sillydevices.patchcore.module.Patch
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.modules.AttenuverterModule
import com.sillydevices.patchcore.modules.ConstModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class ProxyModuleInputTest {

    @Test
    fun testForDoubleEnvelope() {
        val patchCore = TestPatchCore

        class UserInputTestSynth: ModularSynth() {

            inner class InnerModule: PatchModule("inner") {
                val modulation by module(AttenuverterModule("modulation"))
                val input = createInput(modulation.input, "in")
                val output = createOutput(modulation.output, "out")
                val cv = createUserInput(modulation.cv, "cv")
            }

            val bias by module(ConstModule("bias", 1.0f))
            val innerModulation by module(InnerModule())

            val cv = createUserInput(innerModulation.cv, "cv")
            override val defaultPatch: Patch = createPatch {
                patch(bias.output, innerModulation.input)
                patch(innerModulation.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(UserInputTestSynth())

        synth.cv.setValue(0.5f)

        // Simulate user input
        //100 [0.416115, 0.416115]
        //1000 [0.881992, 0.881992]
        SynthTester().testModularSynth(
            synth,
            0.5f, 0.5f,
            100, 44100/10, 0.0001f
        )

        synth.release()
        patchCore.release()
    }

}