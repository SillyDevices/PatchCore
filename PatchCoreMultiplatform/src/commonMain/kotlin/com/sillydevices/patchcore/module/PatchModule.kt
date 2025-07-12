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
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.input.ProxyModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.output.ProxyModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import com.sillydevices.patchcore.module.io.user.proxy.ProxyUserInput
import kotlin.properties.ReadOnlyProperty
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

    //TODO make proxy input and float user input
    protected val inputs = mutableListOf<ProxyModuleInput>()
    protected val outputs = mutableListOf<ProxyModuleOutput>()
    protected val userInputs = mutableListOf<ProxyUserInput>()

//    fun <T:Module> createModule(module: T): T {
//        return registeredModuleInternal(module, null)
//    }

    class ModuleDelegate<T : Module>(patchModule: PatchModule, module: T, settings: ModuleSettings<T>? = null):kotlin.properties.ReadOnlyProperty<PatchModule, T> {
        val result: T = patchModule.registeredModuleInternal(module, settings)

        override fun getValue(patchModule: PatchModule, property: KProperty<*>): T {
            return result
        }
    }

    fun <T:Module> registeredModuleInternal(module: T, settings: ModuleSettings<T>?): T {
        modules.add(module)
        if (settings != null){
            modulesSettings.put(module, settings)
        }
        return module
    }

    protected fun <T : Module> module(module: T, initialSettings: (T.()-> Unit)? = null): ModuleDelegate<T> {
        return ModuleDelegate(this, module, initialSettings?.let { ModuleSettings(it) })
    }

    protected fun <T: Module> create(module: T, initialSettings: (T.()-> Unit)? = null): ModuleDelegate<T> {
        return ModuleDelegate(this, module, initialSettings?.let { ModuleSettings(it) })
    }

    protected  fun <T : Module> moduleCreate( factory : ()-> T): ModuleDelegate<T> {
        return ModuleDelegate(this, factory(), null)
    }

    inline fun <reified T : PatchModule> moduleTest(
        noinline factory: () -> T, noinline initialSettings: (T.()-> Unit)? = null
    ): ReadOnlyProperty<PatchModule, T> {
        return object : ReadOnlyProperty<PatchModule, T> {
            private var instance: T? = null
            override fun getValue(thisRef: PatchModule, property: KProperty<*>): T {
                if (instance != null) return instance!!

                val name = property.name
                val mod = factory()
                thisRef.registeredModuleInternal(mod, null)
                instance = mod
                return mod
            }
        }
    }

    fun createInput(input: ModuleInput, name: String = input.name): ModuleInput {
        val proxy = input.createProxy(moduleName = this.name, withName = name)
        inputs.add(proxy)
        return proxy
    }

    fun createOutput(output: ModuleOutput, name: String = output.name): ModuleOutput {
        val proxy = output.createProxy(moduleName = this.name, withName = name)
        outputs.add(proxy)
        return proxy
    }

    fun <T: UserInput> createUserInput(userInput: T, name: String = userInput.name): T {
        val proxy: T = userInput.createProxy(moduleName = this.name, withName = name)
        userInputs.add(proxy as ProxyUserInput)
        return proxy
    }

    open val defaultPatch: Patch = Patch()

    fun createPatch(builder: PatchBuilder.() -> Unit) : Patch {
        val patchBuilder = PatchBuilder()
        builder.invoke(patchBuilder)
        return patchBuilder.build()
    }

    override fun applyContext(context: ModuleContext) {
        context as? PatchModuleContext ?: throw IllegalArgumentException("Context must be of type PatchModuleContext")
        super.applyContext(context)

        for (module in modules) {
            module.createFrom(context)
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

        for (input in inputs) {
            context.addInput(input.input, input.name)
            val pointer = context.getModuleInputPointer(input)
            input.setPointer(pointer)
        }
        for (output in outputs) {
            context.addOutput(output.output, output.name)
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }
        for (userInput in userInputs) {
            context.addUserInput(userInput.userInput, userInput.name)

            val pointer = context.getUserInputPointer(userInput)
            userInput.applyParentContext(pointer, context)
//            context.addUserInput(userInput.userInput, userInput.name)
//            context.updateUserInput(userInput)
        }

        for (settings in modulesSettings) {
            settings.value.invoke(settings.key)
        }

        for (connection in defaultPatch.connections) {
            context.addPatch(connection.output, connection.input)
        }
    }


    override fun createFrom(parentContext: PatchModuleContext) {
        val pointer = parentContext.createPatchModule(newPatchModule = this)
        val newContext = parentContext.getContextFactory().createPatchModuleContext(pointer, parentContext)
        applyContext(newContext)
    }

}

