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
#include <android/log_macros.h>

#include <string>
#include <PatchCore/module/Module.hpp>

#define LOG_TAG "ModuleJni"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_ModuleJni_moduleGetModuleOutput(
        JNIEnv *env, jobject thiz,jlong module_pointer, jstring output_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    const char* outputNameChars = env->GetStringUTFChars(output_name, nullptr);
    std::string outputNameString(outputNameChars);
    auto result = module->getModuleOutput(outputNameString);
    env->ReleaseStringUTFChars(output_name, outputNameChars);
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_ModuleJni_moduleGetModuleInput(
        JNIEnv *env, jobject thiz, jlong module_pointer, jstring input_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    if (module == nullptr) throw std::runtime_error("Module pointer is null");
    const char* inputNameChars = env->GetStringUTFChars(input_name, nullptr);
    std::string inputNameString(inputNameChars);
    auto result = module->getModuleInput(inputNameString);
    env->ReleaseStringUTFChars(input_name, inputNameChars);
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_ModuleJni_moduleGetUserInput(
        JNIEnv *env, jobject thiz, jlong module_pointer, jstring input_name) {
    auto module = reinterpret_cast<Module *>(module_pointer);
    const char* inputNameChars = env->GetStringUTFChars(input_name, nullptr);
    std::string inputNameString(inputNameChars);
//    ALOGD("-------------------------------------------------------------------");
//    ALOGD("moduleGetUserInput moduleName = %s, inputName = %s", module->getModuleName().c_str(), inputNameChars);
//    ALOGD("module_pointer: %p", module);
    auto result = module->getUserInput(inputNameString);
//    ALOGD("moduleGetUserInput result: %p", result);
    auto userInput = dynamic_cast<UserInput *>(result);
//    ALOGD("moduleGetUserInput userInput: %p", userInput);
    auto test = dynamic_cast<FloatUserInput *>(result);
//    ALOGD("moduleGetUserInput test: %p", test);
//    ALOGD("result type: %s", typeid(result).name());
    env->ReleaseStringUTFChars(input_name, inputNameChars);
    //return reinterpret_cast<jlong>(result);
    auto pointerLong = reinterpret_cast<jlong>(result);
//    ALOGD("Returning pointer: %ld", pointerLong);
    return pointerLong;
}