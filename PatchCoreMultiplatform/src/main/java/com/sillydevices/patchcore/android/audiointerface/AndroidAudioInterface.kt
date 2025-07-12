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

package com.sillydevices.patchcore.android.audiointerface

import com.sillydevices.patchcore.android.NativeObject
import com.sillydevices.patchcore.android.jni.AudioInterfaceJni
import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.audiointerface.AudioInterfaceOptions
import com.sillydevices.patchcore.synth.ModularSynth


class AndroidAudioInterface(
    val pointer: ULong,
): AudioInterface, NativeObject {

    override fun start() {
        AudioInterfaceJni.audioInterfaceStartAudio(pointer)
    }

    override fun stop() {
        AudioInterfaceJni.audioInterfaceStopAudio(pointer)
    }

    override fun isStarted(): Boolean {
        return AudioInterfaceJni.audioInterfaceIsStarted(pointer)
    }

    override fun setSynth(synth: ModularSynth) {
        AudioInterfaceJni.audioInterfaceSetSynth(pointer, synth.pointer.nativePointer)
    }

    fun release() {
        AudioInterfaceJni.audioInterfaceRelease(pointer)
    }

    override fun setOptions(options: AudioInterfaceOptions) {
        if (options is AndroidAudioInterfaceOptions) {
            AudioInterfaceJni.audioInterfaceSetOptions(pointer, options)
        } else {
            error("Unsupported options type: ${options::class}")
        }
    }

}