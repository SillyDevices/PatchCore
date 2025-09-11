# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

project("nativeinterface")

add_library( # Sets the name of the library.
        nativeinterface

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        android/AudioInterface/OboeAudioInterface.cpp
        android/AudioInterface/utils/CpuLoadCounter.cpp
        android/AudioInterface/utils/AudioStreamCallbackProxy.cpp

        jni/DefaultWaveTableProviderJni.cpp
        jni/WaveTableProviderJni.cpp
        jni/DefaultModuleFactoryJni.cpp
        jni/ModuleFactoryJni.cpp
        jni/ModuleJni.cpp
        jni/PatchModuleJni.cpp
        jni/PolyModuleJni.cpp
        jni/UserInputJni.cpp

        jni/ModularSynthJni.cpp
        jni/AudioInterfaceJni.cpp

        jni/modules/KeyboardModuleJni.cpp
        jni/modules/IndicatorModuleJni.cpp

        jni/test/ModularSynthTesterJni.cpp
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_package (oboe REQUIRED CONFIG)
find_package (patchcore REQUIRED CONFIG)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        nativeinterface
        # Links the target library to the log library
        # included in the NDK.
        PatchCore::PatchCore
        oboe::oboe
        ${log-lib})

target_compile_options(nativeinterface PRIVATE -Wall -Ofast -fno-finite-math-only)

#add_library(
#        nativeTestUtils
#        SHARED
#        jni/test/ModularSynthTesterJni.cpp
#)
#
#target_link_libraries(
#        nativeTestUtils
#        # Links the target library to the log library
#        # included in the NDK.
#        PatchCore::PatchCore
#        oboe::oboe
#        ${log-lib}
#)