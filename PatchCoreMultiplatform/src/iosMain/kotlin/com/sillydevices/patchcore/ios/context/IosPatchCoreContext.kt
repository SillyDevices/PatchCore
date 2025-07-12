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

package com.sillydevices.patchcore.ios.context

import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.context.PatchCoreContext
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.ios.audiointerface.IosAudioInterface
import com.sillydevices.patchcore.ios.wrappers.modularSynthCreate
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.synth.ModularSynth
import kotlinx.cinterop.ExperimentalForeignApi

@OptIn(ExperimentalForeignApi::class)
class IosPatchCoreContext(
    private val contextFactory: ContextFactory,
): PatchCoreContext {

    override fun <T : ModularSynth> createSynth(moduleFactory: ModuleFactory, synth: T): T {
        val synthPointer = ModulePointer(
            modularSynthCreate(moduleFactory.pointer.nativePointer, synth.sampleRate)
        )
        val context = IosModularSynthContext(synthPointer, moduleFactory, contextFactory)
        synth.applyContext(context)
        return synth
    }

    override fun <T: AudioInterface> createAudioInterface(): T {
        val result = IosAudioInterface()
        return result as T
    }

}