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

package com.sillydevices.patchcore

import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.context.factory.ModuleContextFactory
import com.sillydevices.patchcore.context.factory.ModuleContextFactoryBuilder
import com.sillydevices.patchcore.context.factory.createContextFactory
import com.sillydevices.patchcore.context.factory.createDefaultModuleContextFactory
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.module.factory.builder.ModuleFactoryBuilder
import com.sillydevices.patchcore.module.factory.builder.createDefaultModuleFactory
import com.sillydevices.patchcore.module.factory.builder.createDefaultWaveTableProvider
import com.sillydevices.patchcore.synth.ModularSynth

class PatchCore(
    moduleFactory: ModuleFactoryBuilder.() -> ModuleFactory = ::defaultModuleFactory,
    moduleContextFactory: ModuleContextFactoryBuilder.() -> ModuleContextFactory = ::defaultModuleContextFactory,
) {
    private object ModuleFactoryBuilderImpl: ModuleFactoryBuilder
    private object ModuleContextFactoryBuilderImpl: ModuleContextFactoryBuilder

    private val contextFactory = createContextFactory(
        moduleContextFactory(ModuleContextFactoryBuilderImpl)
    )
    private val context = contextFactory.createPatchCoreContext()
    private val moduleFactory = moduleFactory(ModuleFactoryBuilderImpl)

    fun createAudioInterface(): AudioInterface {
        return context.createAudioInterface()
    }

    fun <T: ModularSynth> createSynth(synth: T): T {
        return context.createSynth(moduleFactory, synth)
    }

    fun release() {
        moduleFactory.release()
        //TODO wavetable provider release
    }
}



private fun defaultModuleFactory(builder: ModuleFactoryBuilder): ModuleFactory = with(builder){
    val waveTableProvider = createDefaultWaveTableProvider(44100)
    createDefaultModuleFactory(waveTableProvider, null)
}

private fun defaultModuleContextFactory(builder: ModuleContextFactoryBuilder): ModuleContextFactory = with(builder) {
    createDefaultModuleContextFactory()
}