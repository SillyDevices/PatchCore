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

#include <PatchCore/test/ModularSynthTester.hpp>

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_ModularSynthTesterJni_testModularSynth(
        JNIEnv *env, jobject thiz, jlong pointer, jfloat expected_left, jfloat expected_right,
        jint buffer_size, jint samples, jfloat tolerance) {

    auto tester = ModularSynthTester();
    auto modulePtr = reinterpret_cast<Module *>(pointer);
    auto synth = dynamic_cast<ModularSynth *>(modulePtr);
    tester.testSynth(synth, expected_left, expected_right, buffer_size, samples, tolerance);
}