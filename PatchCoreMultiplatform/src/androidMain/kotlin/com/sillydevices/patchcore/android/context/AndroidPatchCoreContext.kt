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

package com.sillydevices.patchcore.android.context

import com.sillydevices.patchcore.android.audiointerface.AndroidAudioInterface
import com.sillydevices.patchcore.android.context.factory.DefaultAndroidContextFactory
import com.sillydevices.patchcore.android.jni.AudioInterfaceJni
import com.sillydevices.patchcore.android.jni.ModularSynthJni
import com.sillydevices.patchcore.android.jni.modules.factory.DefaultModuleFactoryJni
import com.sillydevices.patchcore.context.PatchCoreContext
import com.sillydevices.patchcore.android.modules.factory.AndroidDefaultModuleFactory
import com.sillydevices.patchcore.audiointerface.AudioInterface
import com.sillydevices.patchcore.context.factory.ContextFactory
import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.module.factory.DefaultModuleFactory
import com.sillydevices.patchcore.module.factory.ModuleFactory
import com.sillydevices.patchcore.synth.ModularSynth

class AndroidPatchCoreContext(
    private val contextFactory: ContextFactory,
): PatchCoreContext {

    @Volatile private var isLoaded = false
    @Synchronized
    fun load() {
        if (!isLoaded) {
            System.loadLibrary("nativeinterface")
            isLoaded = true
        }
    }

    init {
        load()
    }

    override fun <T : ModularSynth> createSynth(moduleFactory: ModuleFactory, synth: T): T {
        val synthPointer = ModulePointer(
            ModularSynthJni.modularSynthCreate(moduleFactory.pointer.nativePointer, synth.sampleRate)
        )
        val context = AndroidModularSynthContext(synthPointer, moduleFactory, contextFactory)
        synth.applyContext(context)
        return synth
    }

    override fun <T: AudioInterface> createAudioInterface(): T {
        val pointer = AudioInterfaceJni.audioInterfaceCreate()
        val result = AndroidAudioInterface(pointer)
        return result as T
    }

}