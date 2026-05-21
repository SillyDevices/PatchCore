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

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.sillydevices.patchcore.android.utils.SynthTester
import com.sillydevices.patchcore.android.utils.createPatchCoreForTest
import com.sillydevices.patchcore.module.Patch
import com.sillydevices.patchcore.module.PolyModule
import com.sillydevices.patchcore.modules.PassModule
import com.sillydevices.patchcore.modules.VcaModule
import com.sillydevices.patchcore.modules.vcf.BiQuadVCFModule
import com.sillydevices.patchcore.modules.vco.LfoModule
import com.sillydevices.patchcore.modules.vco.VcoModule
import com.sillydevices.patchcore.synth.ModularSynth
import com.sillydevices.patchcore.types.WaveForm
import kotlin.math.sqrt
import kotlin.system.measureNanoTime
import org.junit.Test
import org.junit.runner.RunWith

//@RunWith(AndroidJUnit4::class)
class PerformanceTest {

    private fun Long.toMs(): Double = this / 1_000_000.0

    private fun List<Double>.median(): Double {
        val middle = size / 2
        return if (size % 2 == 0) {
            (this[middle - 1] + this[middle]) / 2.0
        } else {
            this[middle]
        }
    }

    private fun List<Double>.stdDev(average: Double): Double {
        val variance = sumOf { value ->
            val diff = value - average
            diff * diff
        } / size
        return sqrt(variance)
    }

    private fun logPerformance(message: String) {
        Log.i("PatchCorePerformanceTest", message)
        println("PatchCorePerformanceTest $message")
    }

//    @Test
    fun vcoBiquadVcaLfoOneMillionSamples() {
        val patchCore = createPatchCoreForTest()

        class TestSynth: ModularSynth() {
            inner class PerformanceVoiceModule: PolyModule("performanceVoice", 10) {
                val vco by module {
                    VcoModule(auto) config {
                        userWaveForm.setValue(WaveForm.SAW)
                        userCv.setValue(0.4f)
                    }
                }
                val biquad by module {
                    BiQuadVCFModule(auto) config {
                        userCutoff.setValue(4f)
                        userResonance.setValue(0.35f)
                        type.setValue(BiQuadVCFModule.Type.LOWPASS)
                    }
                }
                val vca by module { VcaModule(auto) }
                val lfo by module {
                    LfoModule(auto) config {
                        userWaveForm.setValue(WaveForm.SIN)
                        userHz.setValue(3f)
                    }
                }

                val passA1 by module { PassModule(auto) }
                val passA2 by module { PassModule(auto) }
                val passA3 by module { PassModule(auto) }
                val passA4 by module { PassModule(auto) }
                val passA5 by module { PassModule(auto) }
                val passA6 by module { PassModule(auto) }
                val passA7 by module { PassModule(auto) }
                val passA8 by module { PassModule(auto) }
                val passA9 by module { PassModule(auto) }
                val passA10 by module { PassModule(auto) }

                val passB1 by module { PassModule(auto) }
                val passB2 by module { PassModule(auto) }
                val passB3 by module { PassModule(auto) }
                val passB4 by module { PassModule(auto) }
                val passB5 by module { PassModule(auto) }
                val passB6 by module { PassModule(auto) }
                val passB7 by module { PassModule(auto) }
                val passB8 by module { PassModule(auto) }
                val passB9 by module { PassModule(auto) }
                val passB10 by module { PassModule(auto) }

                val output by expose(vca.output, "output")

                override val defaultPatch: Patch = createPatch {
                    patch(vco.output, passA1.input)
                    patch(passA1.output, passA2.input)
                    patch(passA2.output, passA3.input)
                    patch(passA3.output, passA4.input)
                    patch(passA4.output, passA5.input)
                    patch(passA5.output, passA6.input)
                    patch(passA6.output, passA7.input)
                    patch(passA7.output, passA8.input)
                    patch(passA8.output, passA9.input)
                    patch(passA9.output, passA10.input)
                    patch(passA10.output, biquad.input)

                    patch(vco.output, passB1.input)
                    patch(passB1.output, passB2.input)
                    patch(passB2.output, passB3.input)
                    patch(passB3.output, passB4.input)
                    patch(passB4.output, passB5.input)
                    patch(passB5.output, passB6.input)
                    patch(passB6.output, passB7.input)
                    patch(passB7.output, passB8.input)
                    patch(passB8.output, passB9.input)
                    patch(passB9.output, passB10.input)
                    patch(passB10.output, biquad.input)

                    patch(lfo.output, biquad.cutoff)
                    patch(lfo.output, vca.cv)
                    patch(biquad.output, vca.input)
                }
            }

            val voice by module { PerformanceVoiceModule() }

            override val defaultPatch: Patch = createPatch {
                patch(voice.output, monoOutput)
            }
        }

        val synth = patchCore.createSynth(TestSynth())

        val samples = 1_000_000
        val runs = 10
        val tester = SynthTester()
        val elapsedNsByRun = LongArray(runs) { runIndex ->
            measureNanoTime {
                tester.testModularSynth(
                    synth,
                    expectedLeft = 0.0f,
                    expectedRight = 0.0f,
                    bufferSize = 100,
                    samples = samples,
                    tolerance = Float.MAX_VALUE
                )
            }.also { elapsedNs ->
                val elapsedMs = elapsedNs.toMs()
                val samplesPerSecond = samples * 1_000_000_000.0 / elapsedNs.toDouble()
                logPerformance("run=${runIndex + 1}/$runs, elapsed=${elapsedMs}ms, samplesPerSecond=$samplesPerSecond")
            }
        }

        val elapsedMsByRun = elapsedNsByRun.map { it.toMs() }.sorted()
        val averageMs = elapsedMsByRun.average()
        val medianMs = elapsedMsByRun.median()
        val minMs = elapsedMsByRun.first()
        val maxMs = elapsedMsByRun.last()
        val stdDevMs = elapsedMsByRun.stdDev(averageMs)
        val medianSamplesPerSecond = samples * 1_000.0 / medianMs

        logPerformance(
            "vcoBiquadVcaLfoOneMillionSamples: runs=$runs, samples=$samples, " +
                    "median=${medianMs}ms, avg=${averageMs}ms, min=${minMs}ms, max=${maxMs}ms, " +
                    "stdDev=${stdDevMs}ms, medianSamplesPerSecond=$medianSamplesPerSecond"
        )

        synth.release()
        patchCore.release()
    }
}