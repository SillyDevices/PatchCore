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
#include <patchcore/module/Module.hpp>
#include <patchcore/modules/input/event/keyboard/KeyboardModule.hpp>


extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_modules_KeyboardModuleJni_keyboardModuleOnEvent(
        JNIEnv *env, jobject thiz, jlong pointer, jboolean is_pressed, jfloat note,
        jfloat velocity) {
    auto *module = reinterpret_cast<Module *>(pointer);
    if (module == nullptr) throw std::runtime_error("KeyboardModuleJni.cpp: Module pointer is null");
    auto *keyboardModule = dynamic_cast<KeyboardModule *>(module);
    if (keyboardModule == nullptr) throw std::runtime_error("KeyboardModuleJni.cpp: KeyboardModule pointer is null or invalid");
    auto event = KeyboardEvent { is_pressed == 1, note, velocity };
    keyboardModule->onEvent(event);
}