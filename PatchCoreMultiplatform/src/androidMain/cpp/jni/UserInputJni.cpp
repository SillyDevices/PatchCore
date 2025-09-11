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

#include <PatchCore/module/input/UserInput.hpp>
#include <PatchCore/module/input/user/FloatUserInput.hpp>
#include <PatchCore/module/input/user/BoolUserInput.hpp>
#include <PatchCore/module/input/user/EnumUserInput.hpp>

#define LOG_TAG "PatchCoreJni"

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_UserInputJni_floatUserInputSetValue(
        JNIEnv *env, jobject thiz, jlong user_input_pointer, jfloat value) {
//    ALOGD("-------------------------------------------------------------------");
//    ALOGD("Setting FloatUserInput pointer: %ld", user_input_pointer);
    auto userInput = reinterpret_cast<UserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
//    ALOGD("UserInput pointer value = %p", userInput);
    auto floatUserInput = reinterpret_cast<FloatUserInput *>(userInput);
    if (floatUserInput == nullptr) throw std::runtime_error("UserInput is not a FloatUserInput");
//    ALOGD("FloatUserInput pointer value = %p", floatUserInput);
//    ALOGD("prev value = %f", floatUserInput->value);
    floatUserInput->setValue(value);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_UserInputJni_enumUserInputSetValue(
        JNIEnv *env, jobject thiz, jlong user_input_pointer, jint value) {
    auto userInput = reinterpret_cast<EnumUserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    userInput->setValue(value);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_UserInputJni_boolUserInputSetValue(
        JNIEnv *env, jobject thiz, jlong user_input_pointer, jboolean value) {
    auto userInput = reinterpret_cast<BoolUserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    userInput->setValue(value);
}
