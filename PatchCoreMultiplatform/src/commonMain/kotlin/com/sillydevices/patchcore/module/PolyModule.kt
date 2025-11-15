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

package com.sillydevices.patchcore.module

import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.context.PolyModuleContext
import com.sillydevices.patchcore.module.dsl.delegates.ModuleDemuxOutputDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleOutputDelegate
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleDemuxOutput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import kotlin.reflect.KProperty

open class PolyModule(name: String, val polyphonyCount: Int = 2): PatchModule(name) {

    override fun createFrom(parentContext: PatchModuleContext) {
        if (parentContext is PolyModuleContext) {
            throw IllegalStateException("Cannot create PolyModule from another PolyModule")
        }
        val pointer = parentContext.createPolyModule(this)
        val newPointer = parentContext.addModule(pointer, name = this.name)
        val newContext = parentContext.getContextFactory().createPolyModuleContext(newPointer, parentContext)
        applyContext(newContext)
    }

    val demuxOutputs = mutableListOf<ModuleDemuxOutput>()

    override fun applyContextForIO(context: PatchModuleContext) {
        super.applyContextForIO(context)
        val context = context as? PolyModuleContext ?: throw IllegalStateException("PolyModule requires PolyModuleContext")
        // Apply context for demux outputs
        demuxOutputs.forEach { output ->
            context.addDemuxOutput(output.output, output.name, output.defaultVoice)
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }

    }

    fun setActiveVoicesCount(count: Int) {
        if (count < 1 || count > polyphonyCount) {
            throw IllegalArgumentException("Active voices count must be in range 1..$polyphonyCount")
        }
        val context = moduleContext as? PolyModuleContext ?: throw IllegalStateException("Module context is not set")
        context.setActiveVoicesCount(count)
    }

    fun getActiveVoicesCount(): Int {
        val context = moduleContext as? PolyModuleContext ?: throw IllegalStateException("Module context is not set")
        return context.getActiveVoicesCount()
    }

    @Deprecated("Use delegate exposeDemux instead", ReplaceWith("exposeDemuxOutput(output, name, defaultVoice)"))
    fun exposeDemuxOutput(output: ModuleOutput, name: String = output.name, defaultVoice: Int): ModuleDemuxOutput =
        ModuleDemuxOutput(output.createProxy(moduleName = this.name, withName = name), defaultVoice = defaultVoice).also { demuxOutputs.add(it) }


    protected fun exposeDemux(output: ModuleOutput, withName: String? = null, defaultVoice: Int = 0): ModuleDemuxOutputDelegateProvider {
        return ModuleDemuxOutputDelegateProvider(output, withName, defaultVoice)
    }

    protected class ModuleDemuxOutputDelegateProvider(private val moduleO: ModuleOutput, private val withName: String? = null, private val defaultVoice: Int = 0) {
        operator fun provideDelegate(thisRef: PolyModule, property: KProperty<*>): ModuleDemuxOutputDelegate {
            val proxy: ModuleDemuxOutput = thisRef.exposeDemuxOutput(moduleO, withName ?: property.name, defaultVoice)
            return ModuleDemuxOutputDelegate(proxy)
        }
    }


}