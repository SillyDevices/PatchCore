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

package com.sillydevices.patchcore.androd.demo.model

import androidx.lifecycle.ViewModel
import com.sillydevices.patchcore.PatchCore
import com.sillydevices.patchcore.androd.demo.ui.DemoVCOInteractorMock
import com.sillydevices.patchcore.androd.demo.ui.DemoVcoInteractor
import com.sillydevices.patchcore.audiointerface.AndroidAudioInterfaceOptions
import com.sillydevices.patchcore.modules.AttenuverterModule
import com.sillydevices.patchcore.modules.envelope.ADSREnvelopeModule
import com.sillydevices.patchcore.modules.envelope.FaderEnvelopeModule
import com.sillydevices.patchcore.modules.VcaModule
import com.sillydevices.patchcore.modules.input.KeyboardModule
import com.sillydevices.patchcore.modules.vcf.BiQuadVCFModule
import com.sillydevices.patchcore.modules.vco.VcoModule
import com.sillydevices.patchcore.synth.ModularSynth
import com.sillydevices.patchcore.types.WaveForm
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.withTimeout


class AudioEngine: CoroutineScope by CoroutineScope(Dispatchers.Default){
    companion object {
        private val audioEngineInstance: AudioEngine by lazy {
            AudioEngine()
        }
        fun get(): AudioEngine = audioEngineInstance
    }

    class DemoSynth: ModularSynth() {

        val vco by module(VcoModule("vco")) {
            userWaveForm.setValue(WaveForm.SIN)
            userPwm.setValue(0.5f)
            userCv.setValue(4f)
        }

        val vcf by module(BiQuadVCFModule("vcf")) {
            userCutoff.setValue(10f)
            userResonance.setValue(0f)
        }

        val adsr by module(ADSREnvelopeModule("envelope")) {
            userSustain.setValue(1f)
            userAttack.setValue(0.02f)
            userRelease.setValue(0.4f)
        }
        val vca by module(VcaModule("vca"))

        val keyboard by module(KeyboardModule("keyboard"))

        val bypassFader by module(FaderEnvelopeModule("bypassFader")) {
            gate.setValue(0f)
        }
        val bypassVca by module(VcaModule("bypassVca"))

        val fader by module(FaderEnvelopeModule("fader"))
        val outputVca by module(VcaModule("outputVca"))

        val volumeMod by module(AttenuverterModule("volumeMod")) {
            cv.setValue(0.8f)
        }

        override val defaultPatch = createPatch {
            keyboard.cv patchTo vco.cv
            vco.output patchTo vcf.input
            //bypass envelope
            vcf.output patchTo bypassVca.input
            bypassFader.output patchTo bypassVca.cv
            bypassVca.output patchTo outputVca.input
            // envelope
            vcf.output patchTo vca.input
            keyboard.gate patchTo adsr.gate

            adsr.output patchTo vca.cv
            vca.output patchTo outputVca.input
            // main fader and vca
            fader.output patchTo outputVca.cv
            outputVca.output patchTo volumeMod.input
            volumeMod.output patchTo monoOutput
        }
    }

    private val patchCore = PatchCore()

    val synth = patchCore.createSynth(DemoSynth())

    private val audioInterface = patchCore.createAudioInterface().apply {
        setOptions(AndroidAudioInterfaceOptions(
            bufferSizeMultiplayer = 10,
            useStabilizedCallback = false,
            useCpuAffinity = false,
            useGameMode = true
        ))
        setSynth(synth)
    }

    private var stopJob: Job? = null

    @Synchronized
    fun start() {
        launch {
            synth.fader.gate.setValue(1f)
            val job = stopJob
            if (job != null && job.isActive) {
                withTimeout(1000) {
                    job.cancel()
                    job.join()
                }
            }
            if (audioInterface.isStarted().not()) {
                audioInterface.start()
            }
        }
    }

    @Synchronized
    fun stop() {
        val job = stopJob
        if (job != null && job.isActive) return
        stopJob = launch {
            delay(200) // debounce delay for activity recreation
            synth.fader.gate.setValue(0f)
            try {
                delay(1000)
            } finally {
                audioInterface.stop()
                stopJob = null
            }
        }
    }

}


class DemoMainActivityViewModel: ViewModel(), DemoVcoInteractor by DemoVCOInteractorMock() {

    val engine = AudioEngine.get()
    val synth = engine.synth

    fun onStartAudio() {
        engine.start()
    }

    fun onStopAudio() {
        engine.stop()
    }

    override val vcoCV = MutableStateFlow(4f)
    override fun setVcoCV(value: Float) {
        vcoCV.value = value
        synth.vco.userCv.setValue(value)
    }

    override val vcoHZ = MutableStateFlow(0f)
    override fun setVcoHZ(value: Float) {
        vcoHZ.value = value
        synth.vco.userHz.setValue(value)
    }

    override val vcoPWM = MutableStateFlow(0.5f)
    override fun setVcoPWM(value: Float) {
        vcoPWM.value = value
        synth.vco.userPwm.setValue(value)
    }

    override val vcoWaveform = MutableStateFlow(WaveForm.SIN)
    override fun setVcoWaveform(value: WaveForm) {
        vcoWaveform.value = value
        synth.vco.userWaveForm.setValue(value)
    }

    override val vcfCutoff = MutableStateFlow(10f)
    override fun setVcfCutoff(value: Float) {
        vcfCutoff.value = value
        synth.vcf.userCutoff.setValue(value)
    }

    override val vcfResonance = MutableStateFlow(0f)
    override fun setVcfResonance(value: Float) {
        vcfResonance.value = value
        synth.vcf.userResonance.setValue(value)
    }

    override fun onPressEnvelope(note: Float) {
        synth.keyboard.onPress(note)
    }

    override fun onReleaseEnvelope(note: Float) {
        synth.keyboard.onRelease(note)
    }

    override val bypassEnvelope = MutableStateFlow(false)
    override fun setBypassEnvelope(value: Boolean) {
        bypassEnvelope.value = value
        if (value) {
            synth.bypassFader.gate.setValue(1f)
        } else {
            synth.bypassFader.gate.setValue(0f)
        }
    }

    override val muted = MutableStateFlow(false)
    override fun setMuted(value: Boolean) {
        muted.value = value
        if (value) {
            synth.fader.gate.setValue(0f)
        } else {
            synth.fader.gate.setValue(1f)
        }
    }

    override val volume = MutableStateFlow(0.8f)
    override fun setVolume(value: Float) {
        volume.value = value
        synth.volumeMod.cv.setValue(value)
    }

}