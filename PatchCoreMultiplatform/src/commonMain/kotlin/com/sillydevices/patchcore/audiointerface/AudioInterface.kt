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

package com.sillydevices.patchcore.audiointerface

import com.sillydevices.patchcore.internal.pointers.AudioInterfacePointer
import com.sillydevices.patchcore.platform.PlatformAudioInterface
import com.sillydevices.patchcore.synth.ModularSynth


interface AudioInterface {

    fun start()
    fun stop()
    fun isStarted(): Boolean

    fun setSynth(synth: ModularSynth)
    fun setOptions(vararg options: AudioInterfaceOptions)
}

class AudioInterfaceImpl(): AudioInterface {

    protected val pointer: AudioInterfacePointer = PlatformAudioInterface.new()

    override fun start() {
        PlatformAudioInterface.startAudio(pointer)
    }

    override fun stop() {
        PlatformAudioInterface.stopAudio(pointer)
    }

    override fun isStarted(): Boolean {
        return PlatformAudioInterface.isStarted(pointer)
    }

    override fun setSynth(synth: ModularSynth) {
        PlatformAudioInterface.setSynth(pointer, synth.pointer)
    }

    override fun setOptions(vararg options: AudioInterfaceOptions) {
        PlatformAudioInterface.setOptions(pointer, options.toList())
    }

}