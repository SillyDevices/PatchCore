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

#include <PatchCore/module/factory/ModuleFactory.hpp>
#include <PatchCore/module/PolyModule.hpp>

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_PolyModuleJni_polyModuleNew(
        JNIEnv *env, jobject thiz, jlong module_factory_pointer, jstring name, jint sample_rate,
        jint polyphony) {
    auto *factory = reinterpret_cast<ModuleFactory *>(module_factory_pointer);
    if (factory == nullptr) throw std::runtime_error("ModuleFactory pointer is null");
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    std::string nameString(nameChars);
    Module *managedModule = new PolyModule(factory, nameString, sample_rate, polyphony);
    env->ReleaseStringUTFChars(name, nameChars);
    return reinterpret_cast<jlong>(managedModule);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_PolyModuleJni_polyModuleRelease(
        JNIEnv *env, jobject thiz, jlong poly_module_pointer) {
    auto modulePtr = reinterpret_cast<Module *>(poly_module_pointer);
    if (modulePtr == nullptr) throw std::runtime_error("PolyModule pointer is null");
    delete modulePtr;
}
