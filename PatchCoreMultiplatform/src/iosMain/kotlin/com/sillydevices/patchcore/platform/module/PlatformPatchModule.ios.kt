package com.sillydevices.patchcore.platform.module

import com.sillydevices.patchcore.internal.pointers.ModuleFactoryPointer
import com.sillydevices.patchcore.internal.pointers.ModuleInputPointer
import com.sillydevices.patchcore.internal.pointers.ModuleOutputPointer
import com.sillydevices.patchcore.internal.pointers.ModulePointer
import com.sillydevices.patchcore.internal.pointers.UserInputPointer
import com.sillydevices.patchcore.ios.wrappers.ModuleParameterWrapper
import com.sillydevices.patchcore.ios.wrappers.patchModuleExposeInput
import com.sillydevices.patchcore.ios.wrappers.patchModuleAddModule
import com.sillydevices.patchcore.ios.wrappers.patchModuleExposeOutput
import com.sillydevices.patchcore.ios.wrappers.patchModuleAddPatch
import com.sillydevices.patchcore.ios.wrappers.patchModuleRemovePatch
import com.sillydevices.patchcore.ios.wrappers.patchModuleExposeUserInput
import com.sillydevices.patchcore.ios.wrappers.patchModuleCreateModule
import com.sillydevices.patchcore.ios.wrappers.patchModuleNew
import com.sillydevices.patchcore.ios.wrappers.patchModuleDelete
import com.sillydevices.patchcore.ios.wrappers.patchModuleGetModule
import com.sillydevices.patchcore.ios.wrappers.patchModuleResetPatch
import com.sillydevices.patchcore.module.factory.ModuleParameter
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.cstr
import kotlinx.cinterop.get
import kotlinx.cinterop.memScoped

@OptIn(ExperimentalForeignApi::class)
actual object PlatformPatchModule {

    actual fun new(moduleFactoryPointer: ModuleFactoryPointer, name: String, sampleRate: Int): ModulePointer {
        return ModulePointer(
        patchModuleNew(
            moduleFactoryPointer.nativePointer,
            name.cstr,
            sampleRate
        ))
    }

    actual fun delete(patchModulePointer: ModulePointer) {
        patchModuleDelete(patchModulePointer.nativePointer)
    }

    actual fun createModule(patchModulePointer: ModulePointer, moduleType: String, moduleName: String, parameters: List<ModuleParameter>): ModulePointer {
        memScoped {
            val paramCount = parameters.size
            val paramsArray = allocArray<ModuleParameterWrapper>(paramCount)
            parameters.forEachIndexed { i, it ->
                paramsArray.get(i).apply {
                    type = it.getType()
                    enumValue = it.getEnum()
                    boolValue = if (it.getBoolean()) 1 else 0
                    floatValue = it.getFloat()
                    key = it.getName().cstr.getPointer(this@memScoped)
                }
            }
            return ModulePointer(
                patchModuleCreateModule(
                    patchModulePointer.nativePointer,
                    moduleType.cstr,
                    moduleName.cstr,
                    paramsArray,
                    paramCount.toULong()
            ))
        }
    }

    actual fun addModule(patchModulePointer: ModulePointer, modulePointer: ModulePointer): ModulePointer{
        return ModulePointer(
            patchModuleAddModule(
                patchModulePointer.nativePointer,
                modulePointer.nativePointer
        ))
    }

    actual fun getModule(patchModulePointer: ModulePointer, moduleName: String): ModulePointer {
        return ModulePointer(
            patchModuleGetModule(
                patchModulePointer.nativePointer,
                moduleName.cstr
        ))
    }

    actual fun exposeInput(patchModulePointer: ModulePointer, inputPointer: ModuleInputPointer, withName: String) {
        patchModuleExposeInput(
            patchModulePointer.nativePointer,
            inputPointer.nativePointer,
            withName.cstr)
    }

    actual fun exposeOutput(patchModulePointer: ModulePointer, outputPointer: ModuleOutputPointer, withName: String) {
        patchModuleExposeOutput(
            patchModulePointer.nativePointer,
            outputPointer.nativePointer,
            withName.cstr)
    }

    actual fun exposeUserInput(patchModulePointer: ModulePointer, userInputPointer: UserInputPointer, withName: String) {
        patchModuleExposeUserInput(
            patchModulePointer.nativePointer,
            userInputPointer.nativePointer,
            withName.cstr)
    }

    actual fun resetPatch(patchModulePointer: ModulePointer) {
        patchModuleResetPatch(patchModulePointer.nativePointer)
    }

    actual fun addPatch(patchModulePointer: ModulePointer, fromOutputPointer: ModuleOutputPointer, toInputPointer: ModuleInputPointer) {
        patchModuleAddPatch(
            patchModulePointer.nativePointer,
            fromOutputPointer.nativePointer,
            toInputPointer.nativePointer)
    }

    actual fun removePatch(
        patchModulePointer: ModulePointer,
        fromOutputPointer: ModuleOutputPointer,
        toInputPointer: ModuleInputPointer
    ) {
        patchModuleRemovePatch(
            patchModulePointer.nativePointer,
            fromOutputPointer.nativePointer,
            toInputPointer.nativePointer)
    }


}