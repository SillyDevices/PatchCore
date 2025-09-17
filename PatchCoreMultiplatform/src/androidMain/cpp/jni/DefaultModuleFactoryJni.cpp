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
#include <PatchCore/module/factory/DefaultModuleFactory.hpp>


extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_modules_factory_DefaultModuleFactoryJni_defaultModuleFactoryNew(
        JNIEnv *env, jobject thiz, jlong wave_table_provider, jlong custom_module_factory_pointer) {
    WaveTableProvider* waveTableProvider = reinterpret_cast<WaveTableProvider *>(wave_table_provider);
    if (waveTableProvider == nullptr) {
        throw std::runtime_error("WaveTableProvider pointer is null");
    }
    ModuleFactory* customModuleFactory = reinterpret_cast<ModuleFactory *>(custom_module_factory_pointer);
    ModuleFactory *factory = new DefaultModuleFactory(waveTableProvider, customModuleFactory);
    jlong result = reinterpret_cast<jlong>(factory);
    return result;
}