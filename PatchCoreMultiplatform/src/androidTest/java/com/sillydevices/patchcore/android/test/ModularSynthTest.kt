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
import com.sillydevices.patchcore.modules.VcaModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class ModularSynthTest {
    @Test
    fun testModularSynthCreateAndRelease() {
        val patchCore = TestPatchCore
        
        class Synth: ModularSynth()

        val synth = patchCore.createSynth(Synth())
        SynthTester().testModularSynth(
            synth,
            0f, 0f,
            100, 100, 0.0001f
        )
        synth.release()
        patchCore.release()
    }

    //PatchModuleContext.createModule(ModuleBuilder)

    @Test
    fun testModularSynthWithBiasModule() {
        val patchCore = TestPatchCore

        class TestSynth: ModularSynth() {
            val bias by module(ConstModule("bias", 0.5f))
            override val defaultPatch: Patch = createPatch {
                patch(bias.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        SynthTester().testModularSynth(
            synth,
            0.5f, 0.5f,
            100, 100, 0.0001f
        )

        synth.release()
        patchCore.release()
    }

    @Test
    fun testModularSynthWithSeveralModules() {
        val patchCore = TestPatchCore

        class TestSynth: ModularSynth() {
            val biasIn by module(ConstModule("biasIn", 0.5f))
            val biasCv by module(ConstModule("biasCv", 1.0f))
            val vca by module(VcaModule("vca"))
            override val defaultPatch: Patch = createPatch {
                patch(biasIn.output, vca.input)
                patch(biasCv.output, vca.cv)
                patch(vca.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        SynthTester().testModularSynth(
            synth,
            0.5f, 0.5f,
            100, 100, 0.0001f
        )

        synth.release()
        patchCore.release()
    }

    @Test
    fun testModularSynthWithUserInput() {
        val patchCore = TestPatchCore

        class UserInputTestSynth: ModularSynth() {
            val bias by module(ConstModule("bias", 1.0f))
            val modulation by module(AttenuverterModule("modulation"))
            override val defaultPatch: Patch = createPatch {
                patch(bias.output, modulation.input)
                patch(modulation.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(UserInputTestSynth())

        synth.modulation.cv.setValue(0.5f)

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


    @Test
    fun testModularSynthWithPatchModule() {
        val patchCore = TestPatchCore

        class UserInputTestSynth: ModularSynth() {

            inner class TestModule: PatchModule("inner") {
                val bias by module(ConstModule("bias", 1.0f))
                val modulation by module(AttenuverterModule("mod"))
                val output = createOutput(modulation.output, "output")
                //val cv = createUserInput("cv", 0f..1f)
                override val defaultPatch = createPatch {
                    patch(bias.output, modulation.input)
                }
            }
            val test by module(TestModule())
            override val defaultPatch: Patch = createPatch {
                patch(test.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(UserInputTestSynth())

        synth.test.modulation.cv.setValue(0.5f)

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

    @Test
    fun testModularSynthWithPatchModuleAndIO() {
        val patchCore = TestPatchCore

        class TestModuleWithOutput: PatchModule("out_module") {
            val biasIn by module(ConstModule("input", 1.0f))
            val biasCv by module(ConstModule("cv", 0.5f))
            val vca by module(VcaModule("vca"))

            val output = createOutput(vca.output, "output")
            override val defaultPatch = createPatch {
                patch(biasIn.output, vca.input)
                patch(biasCv.output, vca.cv)
            }
        }

        class TestModuleWithInput: PatchModule("in_module") {
            val modulation by module(AttenuverterModule("mod"))
            val input = createInput(modulation.input, "input")
            val output = createOutput(modulation.output, "output")
            val cv = createUserInput(modulation.cv, "cv")
        }

        class UserInputTestSynth: ModularSynth() {

            val outModule by module(TestModuleWithOutput())
            val inModule by module(TestModuleWithInput())

            val cv = createUserInput(inModule.cv, "synth_cv")

            override val defaultPatch: Patch = createPatch {
                patch(outModule.output, inModule.input)
                patch(inModule.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(UserInputTestSynth())

        //synth.inModule.cv.setValue(0.5f)
        synth.cv.setValue(0.5f)

        // Simulate user input
        //100 [0.416115, 0.416115]
        //1000 [0.881992, 0.881992]
        SynthTester().testModularSynth(
            synth,
            0.25f, 0.25f,
            100, 44100/10, 0.0001f
        )

        synth.release()
        patchCore.release()
    }
}
