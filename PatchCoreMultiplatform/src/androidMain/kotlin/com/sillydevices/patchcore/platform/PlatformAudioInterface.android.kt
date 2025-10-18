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

package com.sillydevices.patchcore.platform

import com.sillydevices.patchcore.android.jni.AudioInterfaceJni
import com.sillydevices.patchcore.audiointerface.AndroidAudioInterfaceOptions
import com.sillydevices.patchcore.audiointerface.AudioInterfaceOptions
import com.sillydevices.patchcore.audiointerface.DebugInfo
import com.sillydevices.patchcore.internal.pointers.AudioInterfacePointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer


actual object PlatformAudioInterface {
    actual fun new(): AudioInterfacePointer {
        return AudioInterfacePointer(
            AudioInterfaceJni.audioInterfaceNew()
        )
    }

    actual fun delete(pointer: AudioInterfacePointer) {
        AudioInterfaceJni.audioInterfaceDelete(pointer.nativePointer)
    }

    actual fun startAudio(pointer: AudioInterfacePointer) {
        AudioInterfaceJni.audioInterfaceStartAudio(pointer.nativePointer)
    }

    actual fun stopAudio(pointer: AudioInterfacePointer) {
        AudioInterfaceJni.audioInterfaceStopAudio(pointer.nativePointer)
    }

    actual fun isStarted(pointer: AudioInterfacePointer): Boolean {
        return AudioInterfaceJni.audioInterfaceIsStarted(pointer.nativePointer)
    }

    actual fun setSynth(pointer: AudioInterfacePointer, synthPointer: ModulePointer) {
        AudioInterfaceJni.audioInterfaceSetSynth(
            pointer.nativePointer,
            synthPointer.nativePointer
        )
    }

    actual fun setOptions(pointer: AudioInterfacePointer, options: List<AudioInterfaceOptions>) {
        options.filter { it is AndroidAudioInterfaceOptions }.forEach {
            AudioInterfaceJni.audioInterfaceSetOptions(pointer.nativePointer, it as AndroidAudioInterfaceOptions)
        }
    }

    actual fun getDebugInfo(pointer: AudioInterfacePointer): DebugInfo {
        return AudioInterfaceJni.audioInterfaceGetDebugInfo(pointer.nativePointer)
    }
}