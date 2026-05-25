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
import com.sillydevices.patchcore.context.PolyModuleContext
import com.sillydevices.patchcore.module.dsl.delegates.ModuleDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleInputDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleOutputDelegate
import com.sillydevices.patchcore.module.dsl.delegates.ModuleUserInputDelegate
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.input.ExposedModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.output.ExposedModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput
import com.sillydevices.patchcore.module.io.user.proxy.ExposedModuleUserInput
import kotlin.reflect.KProperty

enum class PatchLayer {
    Internal,
    Normal,
    User,
}

enum class PatchPolicy {
    Replace,
    Keep,
    Mix,
}

open class Patch {
    data class Connection(
        val output: ModuleOutput,
        val input: ModuleInput,
        val layer: PatchLayer = PatchLayer.User,
        val policy: PatchPolicy? = null,
    ) {
        val resolvedPolicy: PatchPolicy
            get() = policy ?: defaultPolicyFor(layer)

        companion object {
            fun defaultPolicyFor(layer: PatchLayer): PatchPolicy = when (layer) {
                PatchLayer.Internal -> PatchPolicy.Keep
                PatchLayer.Normal -> PatchPolicy.Replace
                PatchLayer.User -> PatchPolicy.Keep
            }
        }
    }

    open val connections = listOf<Connection>()
}

private class PatchImpl(list: List<Patch.Connection>): Patch() {
    override val connections = list
}

class PatchBuilder {

    val list = mutableListOf<Patch.Connection>()

    class PatchConnectionHandle internal constructor(
        private val updatePolicy: (PatchPolicy) -> Unit,
    ) {
        infix fun policy(policy: PatchPolicy) {
            updatePolicy(policy)
        }
    }

    fun patch(
        output: ModuleOutput,
        input: ModuleInput,
        layer: PatchLayer = PatchLayer.User,
        policy: PatchPolicy? = null,
    ): PatchConnectionHandle {
        val index = list.size
        list.add(Patch.Connection(output, input, layer, policy))
        return PatchConnectionHandle { newPolicy ->
            list[index] = list[index].copy(policy = newPolicy)
        }
    }

    infix fun patch(patch: Pair<ModuleOutput, ModuleInput>) {
        patch(patch.first, patch.second)
    }

    fun import(other: Patch) {
        list.addAll(other.connections)
    }

    infix fun ModuleOutput.internalTo(input: ModuleInput): PatchConnectionHandle {
        return patch(this, input, PatchLayer.Internal)
    }

    infix fun ModuleOutput.normalTo(input: ModuleInput): PatchConnectionHandle {
        return patch(this, input, PatchLayer.Normal)
    }

    infix fun ModuleOutput.userTo(input: ModuleInput): PatchConnectionHandle {
        return patch(this, input, PatchLayer.User)
    }

    infix fun ModuleOutput.connect(input: ModuleInput): PatchConnectionHandle {
        return internalTo(input)
    }

    fun build(): Patch {
        return PatchImpl(list)
    }
}


open class PatchModule(name: String, protected val moduleNamePrefix: String? = null): Module(name) {

    protected data class MutablePatch(
        override val connections: MutableList<Connection> = mutableListOf()
    ): Patch()

    @Suppress("UNCHECKED_CAST")
    class ModuleSettings<T: Module>(val settings: T.() -> Unit) {
        operator fun invoke(module: Module) {
            (module as? T)?.settings() ?: error("Module type mismatch: got ${module::class}")
        }
    }

    protected val modules = mutableListOf<Module>()
    protected val modulesSettings = mutableMapOf<Module, ModuleSettings<*>>()
    val sampleRate: Int = 44100 //TODO: make it configurable

    protected val exposedInputs = mutableListOf<ExposedModuleInput>()
    protected val exposedOutputs = mutableListOf<ExposedModuleOutput>()
    protected val exposedUserInputs = mutableListOf<ExposedModuleUserInput>()

    protected fun <T:Module> registeredModuleInternal(module: T, settings: ModuleSettings<T>?): T {
        modules.add(module)
        if (settings != null){
            modulesSettings.put(module, settings)
        }
        return module
    }

    private fun exposeInput(input: ModuleInput, name: String = input.name): ModuleInput =
        input.createExposed(moduleName = this.name, withName = name).also { exposedInputs.add(it) }

    private fun exposeOutput(output: ModuleOutput, name: String = output.name): ModuleOutput =
        output.createExposed(moduleName = this.name, withName = name).also { exposedOutputs.add(it) }

    private fun <T: UserInput> exposeUserInput(userInput: T, name: String = userInput.name): T =
        userInput.createExposed<T>(moduleName = this.name, withName = name).also { exposedUserInputs.add(it as ExposedModuleUserInput) }

    open val patch: Patch = Patch()

    fun createPatch(builder: PatchBuilder.() -> Unit) : Patch {
        val patchBuilder = PatchBuilder()
        builder.invoke(patchBuilder)
        return patchBuilder.build()
    }

    protected val declaredPatch: MutablePatch = MutablePatch()
    protected val activePatch: MutablePatch = MutablePatch()

    protected open fun clearPatchStorage() {
        val context = moduleContext as PatchModuleContext
        context.resetPatch()
        declaredPatch.connections.clear()
        activePatch.connections.clear()
    }

    protected open fun applyPatch() {
        declaredPatch.connections.clear()
        declaredPatch.connections.addAll(collectPatchConnections())
        rebuildActivePatch()
    }

    protected open fun collectPatchConnections(): List<Patch.Connection> {
        val connections = mutableListOf<Patch.Connection>()
        connections.addAll(patch.connections)
        modules.filterIsInstance<FlatModule>().forEach { flatModule ->
            connections.addAll(flatModule.collectInlinePatchConnections())
        }
        return connections
    }

    open fun addPatch(from: ModuleOutput, to: ModuleInput, layer: PatchLayer = PatchLayer.User, policy: PatchPolicy? = null) {
        addPatch(Patch.Connection(from, to, layer, policy))
    }

    open fun addPatch(connection: Patch.Connection){
        declaredPatch.connections.add(connection)
        rebuildActivePatch()
    }

    open fun removePatch(from: ModuleOutput, to: ModuleInput, layer: PatchLayer? = PatchLayer.User) {
        declaredPatch.connections.removeAll { connection ->
            connection.output == from && connection.input == to && (layer == null || connection.layer == layer)
        }
        rebuildActivePatch()
    }

    open fun removePatch(connection: Patch.Connection){
        declaredPatch.connections.remove(connection)
        rebuildActivePatch()
    }

    open fun replacePatchLayer(layer: PatchLayer, connections: List<Patch.Connection>) {
        declaredPatch.connections.removeAll { it.layer == layer }
        declaredPatch.connections.addAll(connections.map { it.copy(layer = layer) })
        rebuildActivePatch()
    }

    open fun clearPatchLayer(layer: PatchLayer) {
        declaredPatch.connections.removeAll { it.layer == layer }
        rebuildActivePatch()
    }

    open fun replacePatch(patch: Patch) {
        declaredPatch.connections.clear()
        declaredPatch.connections.addAll(patch.connections)
        rebuildActivePatch()
    }

    open fun clearPatch() {
        declaredPatch.connections.clear()
        rebuildActivePatch()
    }

    open fun getPatch(layer: PatchLayer? = null): Patch {
        val connections = if (layer == null) {
            declaredPatch.connections.toList()
        } else {
            declaredPatch.connections.filter { it.layer == layer }
        }
        return PatchImpl(connections)
    }

    open fun getActivePatch(): Patch {
        return PatchImpl(activePatch.connections.toList())
    }

    protected open fun rebuildActivePatch() {
        val context = moduleContext as PatchModuleContext
        context.resetPatch()
        activePatch.connections.clear()

        val userConnections = declaredPatch.connections.filter { it.layer == PatchLayer.User }
        val nextConnections = declaredPatch.connections.filter { connection ->
            when (connection.layer) {
                PatchLayer.Internal -> true
                PatchLayer.User -> true
                PatchLayer.Normal -> when (connection.resolvedPolicy) {
                    PatchPolicy.Replace -> userConnections.none { it.input == connection.input }
                    PatchPolicy.Keep -> true
                    PatchPolicy.Mix -> true
                }
            }
        }

        nextConnections.forEach { connection ->
            context.addPatch(connection.output, connection.input)
            activePatch.connections.add(connection)
        }
    }

    override fun applyContext(context: ModuleContext) {
        context as? PatchModuleContext ?: throw IllegalArgumentException("Context must be of type PatchModuleContext")
        super.applyContext(context)
        applyPatchModuleContext(context)
    }

    open fun applyPatchModuleContext(context: PatchModuleContext) {
        clearPatchStorage()
        applyContextForIO(context)
        applyChildrenSettings()
        applyPatch()
    }

    protected open fun applyContextForIO(context: PatchModuleContext) {
        for (module in modules) {
            module.createFrom(context)
        }
        for (input in exposedInputs) {
            context.exposeInput(input.input, input.name)
            val pointer = if (context.exposedIoAlreadyExists) {
                input.input.pointer
            } else {
                context.getModuleInputPointer(input)
            }
            input.setPointer(pointer)
        }
        for (output in exposedOutputs) {
            context.exposeOutput(output.output, output.name)
            val pointer = context.getModuleOutputPointer(output)
            output.setPointer(pointer)
        }
        for (userInput in exposedUserInputs) {
            context.exposeUserInput(userInput.userInput, userInput.name)
            val pointer = if (context.exposedIoAlreadyExists) {
                userInput.userInput.pointer
            } else {
                context.getUserInputPointer(userInput)
            }
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

    protected fun applyChildrenSettings() {
        for (settings in modulesSettings) {
            settings.value.invoke(settings.key)
        }
    }

    override fun createFrom(parentContext: PatchModuleContext) {
        //can be called only from PatchModuleContext or PolyModuleContext
        val pointer = parentContext.createPatchModule(newPatchModule = this)
        val newPointer = parentContext.addModule(pointer , this.name)
        val newContext = parentContext.getContextFactory().createPatchModuleContext(
            pointer = newPointer,
            parentContext = parentContext,
            exposedIoAlreadyExists = parentContext.exposedIoAlreadyExists || parentContext is PolyModuleContext
        )
        applyContext(newContext)
    }

    // DSL

    class CreateModuleContext<T:Module>(moduleName: String){
        val auto: String = moduleName
        private var settings: ModuleSettings<T>? = null

        @Deprecated(message = "Use config(initialSettings) instead", replaceWith = ReplaceWith("config(initialSettings)"))
        infix fun T.with( initialSettings: T.()->Unit ): T {
            settings = ModuleSettings(initialSettings)
            return this
        }

        infix fun T.config( initialSettings: T.()->Unit ): T {
            settings = ModuleSettings(initialSettings)
            return this
        }

        infix fun T.init( initialSettings: T.()->Unit ): T {
            settings = ModuleSettings(initialSettings)
            return this
        }

        internal fun getSettings(): ModuleSettings<T>? {
            return settings
        }
    }

    protected fun <T : Module> module(createModule: CreateModuleContext<T>.()->T): CreateModuleDelegateProvider<T> {
        return CreateModuleDelegateProvider(createModule)
    }

    @Deprecated("Use module(createModule: CreateModuleContext<T>.()->T) instead", ReplaceWith("module { module.applySettings() }"))
    protected fun <T : Module> module(module: T, initialSettings: (T.()-> Unit)? = null): ModuleDelegateProvider<T> {
        return ModuleDelegateProvider(module, initialSettings?.let { ModuleSettings(it) })
    }

    protected open fun expose(input: ModuleInput, withName: String? = null): ModuleInputDelegateProvider {
        return ModuleInputDelegateProvider(input, withName)
    }

    protected open fun expose(output: ModuleOutput, withName: String? = null): ModuleOutputDelegateProvider {
        return ModuleOutputDelegateProvider(output, withName)
    }

    protected open fun <T: UserInput> expose(userInput: T, withName: String? = null): ModuleUserInputDelegateProvider<T> {
        return ModuleUserInputDelegateProvider(userInput, withName)
    }

    // DSL delegates providers

    protected class ModuleDelegateProvider<T: Module>(private val module: T, private val settings: ModuleSettings<T>? = null) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleDelegate<T> {
            val result = thisRef.registeredModuleInternal(module, settings)
            return ModuleDelegate(result)
        }
    }

    protected class CreateModuleDelegateProvider<T: Module>(private val createModule: CreateModuleContext<T>.()->T) {
        operator fun provideDelegate(thisRef: PatchModule, property: KProperty<*>): ModuleDelegate<T> {
            val moduleName = thisRef.moduleNamePrefix?.let { "${it}${property.name}" } ?: property.name
            val context = CreateModuleContext<T>(moduleName)
            val module = context.createModule()
            val result = thisRef.registeredModuleInternal(module, context.getSettings())
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
