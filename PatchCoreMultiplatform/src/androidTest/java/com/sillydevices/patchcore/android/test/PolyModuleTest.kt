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
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.modules.AttenuverterModule
import com.sillydevices.patchcore.modules.ConstModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class PolyModuleTest {

    @Test
    fun testPolyModularSynthWithBiasModule() {
        val patchCore = TestPatchCore

        class TestSynth: ModularSynth() {



            inner class CustomModule: PolyModule("custom", 4) {
                private val bias by module(ConstModule("bias", 0.5f))
                val output = createOutput(bias.output, "customOutput")
            }

            private val module by module(CustomModule())

            override val defaultPatch: Patch = createPatch {
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        SynthTester().testModularSynth(
            synth,
            2f, 2f,
            100, 100, 0.0001f
        )

        synth.release()
        patchCore.release()
    }


    @Test
    fun testPolyModularSynthWithUserInputModule() {
        val patchCore = TestPatchCore

        class CustomModule: PolyModule("poly", 4) {
            val mod by module(AttenuverterModule("mod"))
            val input = createInput(mod.input, "polyInput")
            val output = createOutput(mod.output, "polyOutput")
            val cv = createUserInput(mod.cv, "polyCv")
        }

        class TestSynth: ModularSynth() {

            private val bias by module(ConstModule("bias", 1.0f))
            val module by module(CustomModule())
            val ctrl = createUserInput(module.cv, "synthCtrl")

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
//        synth.module.mod.userCv.setValue(0.5f)
//        synth.module.cv.setValue(0.5f)
        synth.ctrl.setValue(0.5f)

        SynthTester().testModularSynth(
            synth,
            2f, 2f,
            100, 44100/10, 0.001f
        )

        synth.release()
        patchCore.release()
    }


}