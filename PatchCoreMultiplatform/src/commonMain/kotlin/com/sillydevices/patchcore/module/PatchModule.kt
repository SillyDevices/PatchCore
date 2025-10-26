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

import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.module.dsl.delegates.ModuleDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleInputDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleOutputDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleUserInputDelegate
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.input.ProxyModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.output.ProxyModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import com.sillydevices.patchcore.module.io.user.proxy.ProxyUserInput
import kotlin.reflect.KProperty

open class Patch {
    data class Connection(val output: ModuleOutput, val input: ModuleInput)
    open val connections = listOf<Connection>()
}

private class PatchImpl(list: List<Connection>): Patch() {
    override val connections = list
}

class PatchBuilder {

    val list = mutableListOf<Patch.Connection>()

    fun patch(output: ModuleOutput, input: ModuleInput,) {
        list.add(Patch.Connection(output, input))
    }

    infix fun patch(patch: Pair<ModuleOutput, ModuleInput>) {
        patch(patch.first, patch.second)
    }

    infix fun ModuleOutput.patchTo(input: ModuleInput) {
        patch(this, input)
    }

    infix fun ModuleOutput.connect(input: ModuleInput) {
        patch(this, input)
    }

    fun build(): Patch {
        return PatchImpl(list)
    }
}


open class PatchModule(name: String): Module(name) {

    @Suppress("UNCHECKED_CAST")
    class ModuleSettings<T: Module>(val settings: T.() -> Unit) {
        operator fun invoke(module: Module) {
            (module as? T)?.settings() ?: error("Module type mismatch: got ${module::class}")
        }
    }

    protected val modules = mutableListOf<Module>()
    protected val modulesSettings = mutableMapOf<Module, ModuleSettings<*>>()
    val sampleRate: Int = 44100 //TODO: make it configurable

    protected val proxyInputs = mutableListOf<ProxyModuleInput>()
    protected val proxyOutputs = mutableListOf<ProxyModuleOutput>()
    protected val proxyUserInputs = mutableListOf<ProxyUserInput>()

    protected fun <T:Module> registeredModuleInternal(module: T, settings: ModuleSettings<T>?): T {
        modules.add(module)
        if (settings != null){
            modulesSettings.put(module, settings)
        }
        return module
    }

    fun exposeInput(input: ModuleInput, name: String = input.name): ModuleInput =
        input.createProxy(moduleName = this.name, withName = name).also { proxyInputs.add(it) }

    fun exposeOutput(output: ModuleOutput, name: String = output.name): ModuleOutput =
        output.createProxy(moduleName = this.name, withName = name).also { proxyOutputs.add(it) }

    fun <T: UserInput> exposeUserInput(userInput: T, name: String = userInput.name): T =
        userInput.createProxy<T>(moduleName = this.name, withName = name).also { proxyUserInputs.add(it as ProxyUserInput) }

    open val defaultPatch: Patch = Patch()

    fun createPatch(builder: PatchBuilder.() -> Unit) : Patch {
        val patchBuilder = PatchBuilder()
        builder.invoke(patchBuilder)
        return patchBuilder.build()
    }

    override fun applyContext(context: ModuleContext) {
        context as? PatchModuleContext ?: throw IllegalArgumentException("Context must be of type PatchModuleContext")
        super.applyContext(context)
        createModulesAndIO(context)
        applyContextToChild(context)
    }

    private fun createModulesAndIO(context: PatchModuleContext) {
        for (module in modules) {
            module.createFrom(context)
        }
        for (input in proxyInputs) {
            context.addInput(input.input, input.name)
            val pointer = context.getModuleInputPointer(input)
            input.setPointer(pointer)
        }
        for (output in proxyOutputs) {
            context.addOutput(output.output, output.name)
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }
        for (userInput in proxyUserInputs) {
            context.addUserInput(userInput.userInput, userInput.name)
            val pointer = context.getUserInputPointer(userInput)
            userInput.applyParentContext(pointer, context)
        }
        //this is here only for ModularSynth's outputs
        for (input in registeredInputs) {
            val pointer = context.getModuleInputPointer(input)
            input.setPointer(pointer)
        }
        for (output in registeredOutputs) {
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }
        for (userInput in registeredUserInputs) {
            val pointer = context.getUserInputPointer(userInput)
            userInput.applyParentContext(pointer, context)
        }
        //maybe it can be useful for later
    }

    private fun applyContextToChild(context: PatchModuleContext) {
        for (settings in modulesSettings) {
            settings.value.invoke(settings.key)
        }
        for (connection in defaultPatch.connections) {
            context.addPatch(connection.output, connection.input)
        }
    }

    override fun createFrom(parentContext: PatchModuleContext) {
        //can be called only from PatchModuleContext or PolyModuleContext
        val pointer = parentContext.createPatchModule(newPatchModule = this)
        val newPointer = parentContext.addModule(pointer , this.name)
        val newContext = parentContext.getContextFactory().createPatchModuleContext(newPointer, parentContext)
        applyContext(newContext)
    }

    // DSL

    protected fun <T : Module> module(module: T, initialSettings: (T.()-> Unit)? = null): ModuleDelegateProvider<T> {
        return ModuleDelegateProvider(module, initialSettings?.let { ModuleSettings(it) })
    }

    protected fun expose(input: ModuleInput, withName: String? = null): ModuleInputDelegateProvider {
        return ModuleInputDelegateProvider(input, withName)
    }

    protected fun expose(output: ModuleOutput, withName: String? = null): ModuleOutputDelegateProvider {
        return ModuleOutputDelegateProvider(output, withName)
    }

    protected fun <T: UserInput> expose(userInput: T, withName: String? = null): ModuleUserInputDelegateProvider<T> {
        return ModuleUserInputDelegateProvider(userInput, withName)
    }

    // DSL delegates providers

    protected class ModuleDelegateProvider<T: Module>(private val module: T, private val settings: ModuleSettings<T>? = null) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleDelegate<T> {
            val result = thisRef.registeredModuleInternal(module, settings)
            return ModuleDelegate(result)
        }
    }

    protected class ModuleInputDelegateProvider(private val moduleInput: ModuleInput, private val withName: String? = null) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleInputDelegate {
            val proxy = thisRef.exposeInput(moduleInput, withName ?: property.name)
            return ModuleInputDelegate(proxy)
        }
    }

    protected class ModuleOutputDelegateProvider(private val moduleO: ModuleOutput, private val withName: String? = null) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleOutputDelegate {
            val proxy = thisRef.exposeOutput(moduleO, withName ?: property.name)
            return ModuleOutputDelegate(proxy)
        }
    }

    protected class ModuleUserInputDelegateProvider<T: UserInput>(private val userInput: T, private val withName: String? = null) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleUserInputDelegate<T> {
            val proxy = thisRef.exposeUserInput(userInput, withName ?: property.name)
            return ModuleUserInputDelegate(proxy)
        }
    }

}

