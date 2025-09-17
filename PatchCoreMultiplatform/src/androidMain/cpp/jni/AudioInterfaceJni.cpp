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

#include "../android/AudioInterface/OboeAudioInterface.hpp"
#include <PatchCore/synth/ModularSynth.hpp>


extern "C"
JNIEXPORT jlong JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceNew(
        JNIEnv *env, jobject thiz) {
    OboeAudioInterface* interface = new OboeAudioInterface();
    jlong result = reinterpret_cast<jlong>(interface);
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceDelete(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface *>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    delete audioInterface;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceStartAudio(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface *>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    audioInterface->startAudio();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceStopAudio(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface*>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    audioInterface->stopAudio();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceIsStarted(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface *>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    return audioInterface->isStarted() ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceSetOptions(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer, jobject options) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface*>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    OboeAudioInterface::PlayerOptions playerOptions;
    jclass optionsClass = env->FindClass("com/sillydevices/patchcore/audiointerface/AndroidAudioInterfaceOptions");//env->GetObjectClass(options);
    jfieldID bufferSizeMultiplayerField = env->GetFieldID(optionsClass, "bufferSizeMultiplayer", "I");
    jfieldID useStabilizedCallbackField = env->GetFieldID(optionsClass, "useStabilizedCallback", "Z");
    jfieldID useCpuAffinityField = env->GetFieldID(optionsClass, "useCpuAffinity", "Z");
    jfieldID preferredCpuIdsField = env->GetFieldID(optionsClass, "preferredCpuIds", "[I");
    jfieldID gameModeField = env->GetFieldID(optionsClass, "useGameMode", "Z");
    playerOptions.bufferSizeMultiplayer = env->GetIntField(options, bufferSizeMultiplayerField);
    playerOptions.useStabilizedCallback = env->GetBooleanField(options, useStabilizedCallbackField);
    playerOptions.useCpuAffinity = env->GetBooleanField(options, useCpuAffinityField);
    playerOptions.useGameMode = env->GetBooleanField(options, gameModeField);
    jintArray preferredCpuIdsArray = (jintArray) env->GetObjectField(options, preferredCpuIdsField);
    jint *preferredCpuIds = env->GetIntArrayElements(preferredCpuIdsArray, nullptr);
    int length = env->GetArrayLength(preferredCpuIdsArray);
    playerOptions.preferredCpuIds = std::vector<int>(length);
    for (int i = 0; i < length; ++i) {
        playerOptions.preferredCpuIds[i] = preferredCpuIds[i];
    }
    audioInterface->setOptions(playerOptions);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sillydevices_patchcore_android_jni_AudioInterfaceJni_audioInterfaceSetSynth(
        JNIEnv *env, jobject thiz, jlong audio_interface_pointer, jlong synth_pointer) {
    auto *audioInterface = reinterpret_cast<OboeAudioInterface *>(audio_interface_pointer);
    if (audioInterface == nullptr) throw std::runtime_error("AudioInterface pointer is null");
    if (synth_pointer == 0) {
        audioInterface->setSynth(nullptr);
    } else {
        auto *module = reinterpret_cast<PatchModule *>(synth_pointer);
        ModularSynth *synth = dynamic_cast<ModularSynth *>(module);
        if (synth == nullptr) throw std::runtime_error("Synth pointer is null");
        audioInterface->setSynth(synth);
    }
}
