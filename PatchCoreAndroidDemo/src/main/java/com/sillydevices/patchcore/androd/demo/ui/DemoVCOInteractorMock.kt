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

package com.sillydevices.patchcore.androd.demo.ui

import com.sillydevices.patchcore.types.WaveForm
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow

class DemoVCOInteractorMock: DemoVcoInteractor {
    override val activeCb = MutableStateFlow(Unit)
    override val vcoCV = MutableStateFlow(4f)
    override val vcoHZ = MutableStateFlow(0f)
    override val vcoWaveform = MutableStateFlow(WaveForm.SIN)
    override val vcoPWM = MutableStateFlow(0.5f)

    override fun setVcoCV(value: Float) {
        vcoCV.value = value
    }

    override fun setVcoHZ(value: Float) {
        vcoHZ.value = value
    }

    override fun setVcoPWM(value: Float) {
        vcoPWM.value = value
    }

    override fun setVcoWaveform(value: WaveForm) {
        vcoWaveform.value = value
    }


    override val vcfCutoff = MutableStateFlow(10f)
    override fun setVcfCutoff(value: Float) {
        vcfCutoff.value = value
    }

    override val vcfResonance = MutableStateFlow(0f)
    override fun setVcfResonance(value: Float) {
        vcfResonance.value = value
    }

    override val bypassEnvelope = MutableStateFlow(false)
    override fun setBypassEnvelope(value: Boolean) {
        bypassEnvelope.value = value
    }

    override fun onPressEnvelope(note: Float) {

    }

    override fun onReleaseEnvelope(note: Float) {

    }

    override val muted = MutableStateFlow(false)
    override fun setMuted(value: Boolean) {
        muted.value = value
    }

    override val volume = MutableStateFlow(1f)
    override fun setVolume(value: Float) {
        volume.value = value
    }

}