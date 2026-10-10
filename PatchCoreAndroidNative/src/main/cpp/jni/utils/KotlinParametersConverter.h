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


#ifndef PATCHCORE_KOTLINPARAMETERSCONVERTER_H
#define PATCHCORE_KOTLINPARAMETERSCONVERTER_H

#include <jni.h>
#include <map>
#include <string>

#include <PatchCore/module/ModuleParameter.hpp>


std::map<std::string, ModuleParameter> convertKotlinParametersToNative(JNIEnv *env, jobject parameters) {
    //parameters is a List<ModuleParameter>
    std::map<std::string, ModuleParameter> nativeParameters = {};
    jclass listClass = env->GetObjectClass(parameters);
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    jint size = env->CallIntMethod(parameters, sizeMethod);

    for (int i = 0; i < size; ++i) {
        jobject parameter = env->CallObjectMethod(parameters, getMethod, i);
        jclass parameterClass = env->FindClass("com/sillydevices/patchcore/module/factory/ModuleParameter");//GetObjectClass(parameter);
        //print class name
        jmethodID getClassMethod = env->GetMethodID(parameterClass, "getClass", "()Ljava/lang/Class;");
        jobject classObject = env->CallObjectMethod(parameter, getClassMethod);
        jclass classClass = env->GetObjectClass(classObject);
        jmethodID getNameMethod = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
        jstring className = (jstring) env->CallObjectMethod(classObject, getNameMethod);
        const char *classNameChars = env->GetStringUTFChars(className, nullptr);

        jmethodID getParameterNameMethod = env->GetMethodID(parameterClass, "getName", "()Ljava/lang/String;");
        jstring name = (jstring) env->CallObjectMethod(parameter, getParameterNameMethod);
        auto nameChars = env->GetStringUTFChars(name, nullptr);
        std::string nameString(nameChars);

        jmethodID getTypeMethod = env->GetMethodID(parameterClass, "getType", "()I");
        jint type = env->CallIntMethod(parameter, getTypeMethod);
        switch (type) { // 0 - enum, 1 - float, 2 - bool
            case 0: {
                jmethodID getIntMethod = env->GetMethodID(parameterClass, "getEnum", "()I");
                int intValue = env->CallIntMethod(parameter, getIntMethod);
                ModuleParameter moduleParameter = ModuleParameter(intValue);
                nativeParameters.emplace(nameString, moduleParameter);
                break;
            }
            case 1: {
                jmethodID getFloatMethod = env->GetMethodID(parameterClass, "getFloat", "()F");
                float floatValue = env->CallFloatMethod(parameter, getFloatMethod);
                ModuleParameter moduleParameter = ModuleParameter(floatValue);
                nativeParameters.emplace(nameString, moduleParameter);
                break;
            }
            case 2: {
                jmethodID getBoolMethod = env->GetMethodID(parameterClass, "getBoolean", "()Z");
                bool boolValue = env->CallBooleanMethod(parameter, getBoolMethod);
                ModuleParameter moduleParameter = ModuleParameter(boolValue);
                nativeParameters.emplace(nameString, moduleParameter);
                break;
            }

        }
        env->ReleaseStringUTFChars(name, nameChars);
    }

    return nativeParameters;
}



#endif //PATCHCORE_KOTLINPARAMETERSCONVERTER_H
