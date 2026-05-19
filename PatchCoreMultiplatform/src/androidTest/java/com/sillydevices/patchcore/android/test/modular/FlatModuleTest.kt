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

package com.sillydevices.patchcore.android.test.modular

import com.sillydevices.patchcore.android.utils.BaseTest
import com.sillydevices.patchcore.android.utils.SynthTester
import com.sillydevices.patchcore.module.FlatModule
import com.sillydevices.patchcore.module.Patch
import com.sillydevices.patchcore.module.PatchModule
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.modules.ConstModule
import com.sillydevices.patchcore.modules.PassModule
import com.sillydevices.patchcore.modules.VcaModule
import com.sillydevices.patchcore.synth.ModularSynth
import org.junit.Assert.assertNotEquals
import org.junit.Test

class FlatModuleTest: BaseTest() {

    @Test
    fun testFlatModule() {


        class TestSynth: ModularSynth() {
            val source by module { ConstModule("source", 0.5f) }

            inner class NestedModule(name: String): FlatModule(name) {
                val const by module { ConstModule("bias", 1.0f) }
                val vca by module { VcaModule("vca") }
                val passModule by module { PassModule("test") }
                override val defaultPatch = createPatch {
                    const.output patchTo vca.cv
                    vca.output patchTo passModule.input
                }

            }

            inner class Engine(name: String): PolyModule(name, 1) {

                private val passIn by module { PassModule(auto) }
                private val passOut by module { PassModule(auto) }

                val nested by module { NestedModule(auto) }

                val input by expose(passIn.input, "engine_input")
                val output by expose(passOut.output, "engine_output")

                val demuxPass by module { PassModule("demux_pass") }
                val testMux by expose(nested.vca.output)
                val testDemux by exposeDemux(nested.vca.output)

                override val defaultPatch = createPatch {
                    passIn.output patchTo nested.vca.input
                    nested.passModule.output patchTo passOut.input
                    nested.passModule.output patchTo demuxPass.input
                    import(nested.defaultPatch)
                }
            }
            val engine by module { Engine(auto) }

            override val defaultPatch: Patch = createPatch {
                source.output patchTo engine.input
                engine.output patchTo monoOutput
            }

        }

        val result = 0.5f

        val synth = patchCore.createSynth(TestSynth())
        assertNotEquals(
            synth.engine.testMux.pointer.nativePointer,
            synth.engine.nested.vca.output.pointer.nativePointer
        )
        assertNotEquals(
            synth.engine.testDemux.pointer.nativePointer,
            synth.engine.nested.vca.output.pointer.nativePointer
        )
        assertNotEquals(
            synth.engine.testMux.pointer.nativePointer,
            synth.engine.testDemux.pointer.nativePointer
        )
        SynthTester().testModularSynth(
            synth,
            result, result,
            100, 44100/10, 0.0001f
        )
        synth.release()
    }

}
