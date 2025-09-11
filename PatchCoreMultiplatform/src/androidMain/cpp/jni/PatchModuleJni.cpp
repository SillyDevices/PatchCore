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


#include <jni.h>

#include "utils/KotlinParametersConverter.h"

#include <PatchCore/module/factory/ModuleFactory.hpp>
#include <PatchCore/module/PatchModule.hpp>

#include <android/log_macros.h>

#define LOG_TAG "PatchModuleJni"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleNew(
        JNIEnv *env, jobject thiz, jlong module_factory_pointer, jstring name, jint sample_rate) {
    auto *factory = reinterpret_cast<ModuleFactory *>(module_factory_pointer);
    if (factory == nullptr) throw std::runtime_error("ModuleFactory pointer is null");
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    std::string nameString(nameChars);
    PatchModule *patchModule = new PatchModule(factory, nameString, sample_rate);
    env->ReleaseStringUTFChars(name, nameChars);
    return reinterpret_cast<jlong>(static_cast<Module*>(patchModule));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleRelease(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer) {
    auto modulePtr = reinterpret_cast<Module *>(patch_module_pointer);
    if (modulePtr == nullptr) throw std::runtime_error("PatchModule pointer is null");
    delete modulePtr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleCreateModule(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jstring module_type,
        jstring module_name, jobject parameters) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    const char *moduleTypeNameChars = env->GetStringUTFChars(module_type, nullptr);
    const char *moduleNameChars = env->GetStringUTFChars(module_name, nullptr);

    std::string moduleTypeNameString(moduleTypeNameChars);
    std::string moduleNameString(moduleNameChars);

    std::map<std::string, ModuleParameter> nativeParameters = convertKotlinParametersToNative(env, parameters);

    auto result = patchModule->createModule(moduleTypeNameString, moduleNameString, nativeParameters);

    env->ReleaseStringUTFChars(module_type, moduleTypeNameChars);
    env->ReleaseStringUTFChars(module_name, moduleNameChars);
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleAddModule(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jlong managed_module_pointer) {
    //remove pointer to managed_module_pointer from java object after this call, or just do not delete this poiner
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    auto *managedModulePtr = reinterpret_cast<Module *>(managed_module_pointer);
    if (managedModulePtr == nullptr) throw std::runtime_error("ManagedModule pointer is not a ManagedNativePtr<Module>");
    patchModule->addModule(std::unique_ptr<Module>(managedModulePtr));
}

extern "C"
JNIEXPORT jlong JNICALL
        Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleGetModule(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jstring module_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    const char *moduleNameChars = env->GetStringUTFChars(module_name, nullptr);
    std::string moduleNameString(moduleNameChars);
    Module *result = patchModule->getModule(moduleNameString);
    env->ReleaseStringUTFChars(module_name, moduleNameChars);
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleAddInput(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jlong input_pointer,
        jstring input_name) {
    ALOGD("-------------------------------------------------------------------");
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    auto input = reinterpret_cast<ModuleInput *>(input_pointer);
    if (input == nullptr) throw std::runtime_error("Input pointer is null");
    const char *inputNameChars = env->GetStringUTFChars(input_name, nullptr);
    std::string inputNameString(inputNameChars);
    patchModule->addInput(input, inputNameString);
    env->ReleaseStringUTFChars(input_name, inputNameChars);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleAddOutput(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jlong output_pointer,
        jstring output_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    auto output = reinterpret_cast<ModuleOutput *>(output_pointer);
    if (output == nullptr) throw std::runtime_error("Output pointer is null");
    const char *outputNameChars = env->GetStringUTFChars(output_name, nullptr);
    std::string outputNameString(outputNameChars);
    patchModule->addOutput(output, outputNameString);
    env->ReleaseStringUTFChars(output_name, outputNameChars);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleAddUserInput(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jlong user_input_pointer,
        jstring input_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    auto userInput = reinterpret_cast<UserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    const char *inputNameChars = env->GetStringUTFChars(input_name, nullptr);
//    ALOGD("PatchModuleAddUserInput moduleName = %s, inputName = %s", module->getModuleName().c_str(), inputNameChars);
//    ALOGD("PatchModuleAddUserInput userInputPointer: %lld", user_input_pointer);
//    ALOGD("PatchModuleAddUserInput userInput: %p", userInput);
    std::string inputNameString(inputNameChars);
    patchModule->addUserInput(userInput, inputNameString);
    env->ReleaseStringUTFChars(input_name, inputNameChars);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleAddPatch(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer, jlong from_output_pointer,
        jlong to_input_pointer) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    auto fromOutput = reinterpret_cast<ModuleOutput *>(from_output_pointer);
    if (fromOutput == nullptr) throw std::runtime_error("From output pointer is null");
    auto toInput = reinterpret_cast<ModuleInput *>(to_input_pointer);
    if (toInput == nullptr) throw std::runtime_error("To input pointer is null");
    patchModule->addPatch(fromOutput, toInput);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PatchModuleJni_patchModuleResetPatch(
        JNIEnv *env, jobject thiz, jlong patch_module_pointer) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    patchModule->resetPatch();
}