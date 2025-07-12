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

package com.sillydevices.patchcore.ios.audiointerface

import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.audiointerface.AudioInterfaceOptions
import com.sillydevices.patchcore.ios.NativeObject
import com.sillydevices.patchcore.synth.ModularSynth
import com.sillydevices.patchcore.ios.wrappers.*
import kotlinx.cinterop.ExperimentalForeignApi


@OptIn(ExperimentalForeignApi::class)
class IosAudioInterface(): AudioInterface, NativeObject {

    protected val _interface = CommonAudioInterface()

    override fun start() {
        _interface.startAudio()
    }

    override fun stop() {
        _interface.stopAudio()
    }

    override fun isStarted(): Boolean {
        return _interface.isStarted()
    }

    override fun setSynth(synth: ModularSynth) {
        _interface.setModularSynth(synth.pointer.nativePointer)
    }

    override fun setOptions(options: AudioInterfaceOptions) {
        if (options is IosAudioInterfaceOptions) {
            // There is no options here
        } else {
            throw IllegalArgumentException("Expected IosAudioInterfaceOptions, got ${options::class.simpleName}")
        }
    }

}