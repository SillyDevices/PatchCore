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

data class AndroidAudioInterfaceOptions(
    val preferredCpuIds: IntArray = intArrayOf(),
    val bufferSizeMultiplayer: Int = 2,
    val useStabilizedCallback: Boolean = true,
    val useCpuAffinity: Boolean = true,
    val useGameMode: Boolean = true
): AudioInterfaceOptions {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other == null || (this::class != other::class)) {
            return false
        }

        other as AndroidAudioInterfaceOptions

        if (!preferredCpuIds.contentEquals(other.preferredCpuIds)) return false
        if (bufferSizeMultiplayer != other.bufferSizeMultiplayer) return false
        if (useStabilizedCallback != other.useStabilizedCallback) return false
        if (useCpuAffinity != other.useCpuAffinity) return false
        if (useGameMode != other.useGameMode) return false

        return true
    }

    override fun hashCode(): Int {
        var result = preferredCpuIds.contentHashCode()
        result = 31 * result + bufferSizeMultiplayer
        result = 31 * result + useStabilizedCallback.hashCode()
        result = 31 * result + useCpuAffinity.hashCode()
        result = 31 * result + useGameMode.hashCode()
        return result
    }
}