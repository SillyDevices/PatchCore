/*
 * PatchCore — Modular Synthesizer Engine
 * Copyright (c) 2026 Evgenii Petrov
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
import com.sillydevices.patchcore.android.utils.BaseTest
import com.sillydevices.patchcore.android.utils.SynthTester
import com.sillydevices.patchcore.module.FlatModule
import com.sillydevices.patchcore.module.Patch
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.modules.AttenuverterModule
import com.sillydevices.patchcore.modules.ConstModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.Assert.assertNotEquals
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class ExposeIoTest : BaseTest() {

    private fun assertExposedPointersAreDistinct(
        exposedInputPointer: ULong,
        sourceInputPointer: ULong,
        exposedOutputPointer: ULong,
        sourceOutputPointer: ULong,
        exposedUserInputPointer: ULong,
        sourceUserInputPointer: ULong
    ) {
        assertNotEquals(exposedInputPointer, sourceInputPointer)
        assertNotEquals(exposedOutputPointer, sourceOutputPointer)
        assertNotEquals(exposedUserInputPointer, sourceUserInputPointer)
    }

    @Test
    fun testExposeIoInPatchModule() {
        class ExposedPatchModule : PatchModule("patch") {
            val modulation by module(AttenuverterModule("modulation"))
            val input by expose(modulation.input, "in")
            val output by expose(modulation.output, "out")
            val cv by expose(modulation.cv, "cv")
        }

        class TestSynth : ModularSynth() {
            val bias by module(ConstModule("bias", 1f))
            val module by module(ExposedPatchModule())

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        assertExposedPointersAreDistinct(
            synth.module.input.pointer.nativePointer,
            synth.module.modulation.input.pointer.nativePointer,
            synth.module.output.pointer.nativePointer,
            synth.module.modulation.output.pointer.nativePointer,
            synth.module.cv.pointer.nativePointer,
            synth.module.modulation.cv.pointer.nativePointer
        )
        synth.module.cv.setValue(0.5f)
        SynthTester().testModularSynth(
            synth,
            0.5f, 0.5f,
            100, 44100 / 10, 0.0001f
        )
        synth.release()
    }

    @Test
    fun testExposeIoInPatchModuleWithFlatModule() {
        class NestedFlatModule(name: String) : FlatModule(name) {
            val modulation by module(AttenuverterModule("modulation"))
        }

        class ExposedPatchModule : PatchModule("patch") {
            val nested by module(NestedFlatModule("nested"))
            val input by expose(nested.modulation.input, "in")
            val output by expose(nested.modulation.output, "out")
            val cv by expose(nested.modulation.cv, "cv")
        }

        class TestSynth : ModularSynth() {
            val bias by module(ConstModule("bias", 1f))
            val module by module(ExposedPatchModule())

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        assertExposedPointersAreDistinct(
            synth.module.input.pointer.nativePointer,
            synth.module.nested.modulation.input.pointer.nativePointer,
            synth.module.output.pointer.nativePointer,
            synth.module.nested.modulation.output.pointer.nativePointer,
            synth.module.cv.pointer.nativePointer,
            synth.module.nested.modulation.cv.pointer.nativePointer
        )
        synth.module.cv.setValue(0.5f)
        SynthTester().testModularSynth(
            synth,
            0.5f, 0.5f,
            100, 44100 / 10, 0.0001f
        )
        synth.release()
    }

    @Test
    fun testExposeIoInPolyModule() {
        class ExposedPolyModule : PolyModule("poly", 4) {
            val modulation by module(AttenuverterModule("modulation"))
            val input by expose(modulation.input, "in")
            val output by expose(modulation.output, "out")
            val cv by expose(modulation.cv, "cv")
        }

        class TestSynth : ModularSynth() {
            val bias by module(ConstModule("bias", 1f))
            val module by module(ExposedPolyModule())

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        assertExposedPointersAreDistinct(
            synth.module.input.pointer.nativePointer,
            synth.module.modulation.input.pointer.nativePointer,
            synth.module.output.pointer.nativePointer,
            synth.module.modulation.output.pointer.nativePointer,
            synth.module.cv.pointer.nativePointer,
            synth.module.modulation.cv.pointer.nativePointer
        )
        synth.module.cv.setValue(0.5f)
        SynthTester().testModularSynth(
            synth,
            2f, 2f,
            100, 44100 / 10, 0.001f
        )
        synth.release()
    }

    @Test
    fun testExposeIoInPolyModuleWithPatchModule() {
        class NestedPatchModule : PatchModule("nestedPatch") {
            val modulation by module(AttenuverterModule("modulation"))
            val input by expose(modulation.input, "in")
            val output by expose(modulation.output, "out")
            val cv by expose(modulation.cv, "cv")
        }

        class ExposedPolyModule : PolyModule("poly", 4) {
            val nested by module(NestedPatchModule())
            val input by expose(nested.input, "polyIn") // тут падает // этот input передается как exposedModuleInput
            val output by expose(nested.output, "polyOut")
            val cv by expose(nested.cv, "polyCv")
        }

        class TestSynth : ModularSynth() {
            val bias by module(ConstModule("bias", 1f))
            val module by module(ExposedPolyModule())

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        assertExposedPointersAreDistinct(
            synth.module.input.pointer.nativePointer,
            synth.module.nested.input.pointer.nativePointer,
            synth.module.output.pointer.nativePointer,
            synth.module.nested.output.pointer.nativePointer,
            synth.module.cv.pointer.nativePointer,
            synth.module.nested.cv.pointer.nativePointer
        )
        synth.module.cv.setValue(0.5f)
        SynthTester().testModularSynth(
            synth,
            2f, 2f,
            100, 44100 / 10, 0.001f
        )
        synth.release()
    }

    @Test
    fun testExposeIoInPolyModuleWithFlatModule() {
        class ExposedPolyModule : PolyModule("poly", 4) {
            val inputPass by module(com.sillydevices.patchcore.modules.PassModule("inputPass"))
            val modulation by module(AttenuverterModule("modulation"))
            val outputPass by module(com.sillydevices.patchcore.modules.PassModule("outputPass"))

            inner class NestedFlatModule(name: String) : FlatModule(name) {
                override val defaultPatch: Patch = createPatch {
                    inputPass.output patchTo modulation.input
                    modulation.output patchTo outputPass.input
                }
            }

            val nested by module(NestedFlatModule("nested"))
            val input by expose(inputPass.input, "polyIn")
            val output by expose(outputPass.output, "polyOut")
            val cv by expose(modulation.cv, "polyCv")

            override val defaultPatch: Patch = createPatch {
                import(nested.defaultPatch)
            }
        }

        class TestSynth : ModularSynth() {
            val bias by module(ConstModule("bias", 1f))
            val module by module(ExposedPolyModule())

            override val defaultPatch: Patch = createPatch {
                patch(bias.output, module.input)
                patch(module.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())
        assertExposedPointersAreDistinct(
            synth.module.input.pointer.nativePointer,
            synth.module.inputPass.input.pointer.nativePointer,
            synth.module.output.pointer.nativePointer,
            synth.module.outputPass.output.pointer.nativePointer,
            synth.module.cv.pointer.nativePointer,
            synth.module.modulation.cv.pointer.nativePointer
        )
        synth.module.cv.setValue(0.5f)
        SynthTester().testModularSynth(
            synth,
            2f, 2f,
            100, 44100 / 10, 0.001f
        )
        synth.release()
    }
}
