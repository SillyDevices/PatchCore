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

package com.sillydevices.patchcore.context

import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.audiointerface.AudioInterfaceImpl
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.platform.PlatformLibraryLoader
import com.sillydevices.patchcore.platform.PlatformModularSynth
import com.sillydevices.patchcore.synth.ModularSynth
import kotlin.concurrent.Volatile


//Main context of the Library, should contain main classes constructors and destructors
interface PatchCoreContext {

    fun createAudioInterface(): AudioInterface

    fun <T: ModularSynth> createSynth(moduleFactory: ModuleFactory, synth: T): T
}


class PatchCoreContextImpl(
    private val contextFactory: ContextFactory,
): PatchCoreContext {

    @Volatile
    private var isLoaded = false

    init {
        if (!isLoaded) {
            isLoaded = PlatformLibraryLoader.load()
        }
    }

    override fun createAudioInterface(): AudioInterface {
        return AudioInterfaceImpl()
    }

    override fun <T: ModularSynth> createSynth(moduleFactory: ModuleFactory, synth: T): T {
        val synthPointer = PlatformModularSynth.create(moduleFactory.pointer, synth.sampleRate)
        val context = ModularSynthContextImpl(synthPointer, moduleFactory, contextFactory)
        //important: apply context before returning
        synth.applyContext(context)
        return synth
    }

}