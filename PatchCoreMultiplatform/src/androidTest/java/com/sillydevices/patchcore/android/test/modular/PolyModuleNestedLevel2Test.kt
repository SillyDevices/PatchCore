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

package com.sillydevices.patchcore.android.test.modular

import com.sillydevices.patchcore.PatchCore
import com.sillydevices.patchcore.android.utils.SynthTester
import com.sillydevices.patchcore.android.utils.createPatchCoreForTest
import com.sillydevices.patchcore.module.Patch
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.modules.AttenuverterModule
import com.sillydevices.patchcore.modules.ConstModule
import com.sillydevices.patchcore.modules.VcaModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.After
import org.junit.Before
import org.junit.Test

class PolyModuleNestedLevel2Test {

    lateinit var patchCore: PatchCore

    @Before
    fun setup() {
        patchCore = createPatchCoreForTest()
    }

    @After
    fun tearDown() {
        patchCore.release()
    }

    @Test
    fun testNestedPatchModuleWithPolyphone3Step() {
        class TestSynth: ModularSynth() {

            val source by module(ConstModule("source", 0.5f))

            inner class InnerInnerNestedModule(name: String): PatchModule(name) {
                val constCv by module(ConstModule("bias", 0.5f))
                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "inner_nested_in")
                val output by expose(vca.output, "inner_nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo vca.cv
                }
            }

            inner class InnerNestedModule(name: String): PatchModule(name) {

                val constCv by module(ConstModule("bias", 0.5f))
                val innerNested by module(InnerInnerNestedModule("inner_inner_nested"))
                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "inner_nested_in")
                val output by expose(vca.output, "inner_nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo innerNested.input
                    innerNested.output patchTo vca.cv
                }
            }

            inner class NestedModule(name: String): PolyModule(name, 1) {

                val constCv by module(ConstModule("bias", 0.5f))
                val innerNested by module(InnerNestedModule("inner_nested"))
                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "nested_in")
                val output by expose(vca.output, "nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo innerNested.input
                    innerNested.output patchTo vca.cv
                }
            }

            val nested by module(NestedModule("nested"))

            override val defaultPatch: Patch = createPatch {
                source.output patchTo nested.input
                nested.output patchTo monoOutput
            }
        }

        val result = 0.5f * 0.5f * 0.5f * 0.5f

        val synth = patchCore.createSynth(TestSynth())
        SynthTester().testModularSynth(
            synth,
            result, result,
            100, 44100/10, 0.0001f
        )

        synth.release()
    }

    @Test
    fun testNestedPatchModuleAndUserInputWithPolyphone3Step() {
        class TestSynth: ModularSynth() {

            val source by module(ConstModule("source", 0.5f))

            inner class InnerInnerNestedModule(name: String): PatchModule(name) {
                val constCv by module(ConstModule("bias", 1f))
                val attenuator by module (AttenuverterModule("att")) {
                    cv.setValue(0.2f)
                }

                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "inner_nested_in")
                val output by expose(vca.output, "inner_nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo attenuator.input
                    attenuator.output patchTo vca.cv
                }
            }

            inner class InnerNestedModule(name: String): PatchModule(name) {

                val constCv by module(ConstModule("bias", 1f))
                val attenuator by module(AttenuverterModule("att")) {
                    cv.setValue(0.3f)
                }
                val innerNested by module(InnerInnerNestedModule("inner_inner_nested"))
                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "inner_nested_in")
                val output by expose(vca.output, "inner_nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo attenuator.input
                    attenuator.output patchTo innerNested.input
                    innerNested.output patchTo vca.cv
                }
            }

            inner class NestedModule(name: String): PolyModule(name, 1) {

                val constCv by module(ConstModule("bias", 1f))
                val attenuator by module(AttenuverterModule("att")) {
                    cv.setValue(0.4f)
                }
                val innerNested by module(InnerNestedModule("inner_nested"))
                val vca by module(VcaModule("vca"))

                val input by expose(vca.input, "nested_in")
                val output by expose(vca.output, "nested_out")

                override val defaultPatch = createPatch {
                    constCv.output patchTo attenuator.input
                    attenuator.output patchTo innerNested.input
                    innerNested.output patchTo vca.cv
                }
            }

            val nested by module(NestedModule("nested"))

            override val defaultPatch: Patch = createPatch {
                source.output patchTo nested.input
                nested.output patchTo monoOutput
            }
        }


        val synth = patchCore.createSynth(TestSynth())

        //        val result = 0.5f * 0.4f * 0.3f * 0.2f
        val result = 0.5f * 0.5f * 0.5f * 0.5f
        synth.nested.attenuator.cv.setValue(0.5f)
        synth.nested.innerNested.attenuator.cv.setValue(0.5f)
        synth.nested.innerNested.innerNested.attenuator.cv.setValue(0.5f)


        SynthTester().testModularSynth(
            synth,
            result, result,
            100, 44100/10, 0.0001f
        )

        synth.release()
    }
}