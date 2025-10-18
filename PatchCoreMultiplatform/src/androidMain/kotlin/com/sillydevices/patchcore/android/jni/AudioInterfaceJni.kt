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

package com.sillydevices.patchcore.android.jni

import com.sillydevices.patchcore.audiointerface.AndroidAudioInterfaceOptions
import com.sillydevices.patchcore.audiointerface.DebugInfo

object AudioInterfaceJni {
    @JvmName("audioInterfaceNew")
    external fun audioInterfaceNew(): ULong
    @JvmName("audioInterfaceDelete")
    external fun audioInterfaceDelete(audioInterfacePointer: ULong)
    @JvmName("audioInterfaceStartAudio")
    external fun audioInterfaceStartAudio(audioInterfacePointer: ULong)
    @JvmName("audioInterfaceStopAudio")
    external fun audioInterfaceStopAudio(audioInterfacePointer: ULong)
    @JvmName("audioInterfaceIsStarted")
    external fun audioInterfaceIsStarted(audioInterfacePointer: ULong): Boolean
    @JvmName("audioInterfaceSetOptions")
    external fun audioInterfaceSetOptions(audioInterfacePointer: ULong, options: AndroidAudioInterfaceOptions)
    @JvmName("audioInterfaceSetSynth")
    external fun audioInterfaceSetSynth(audioInterfacePointer: ULong, synthPointer: ULong)
    @JvmName("audioInterfaceGetDebugInfo")
    external fun audioInterfaceGetDebugInfo(audioInterfacePointer: ULong): DebugInfo
}