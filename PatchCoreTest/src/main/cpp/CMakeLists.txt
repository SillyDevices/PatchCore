cmake_minimum_required(VERSION 3.20.0)

project("patchcoretest")

# Google Test
add_subdirectory(gtest)
enable_testing()

include_directories("gtest/googletest/include/")

# Tests
add_library(
        patchcoretest

        SHARED
        jni_runner.cpp

        test/wavetableProviderTest.cpp
        test/factoryTest.cpp
        test/moduleTest.cpp
        test/synthTest.cpp
        test/patchModuleTest.cpp
        test/keyboardTest.cpp
        test/copyModuleTest.cpp
        test/polyModuleTest.cpp
)

find_package (PatchCore REQUIRED CONFIG)
find_library(log-lib log)

target_link_libraries(
        patchcoretest

        PatchCore::PatchCore
        gtest
        gtest_main
        ${log-lib})

#target_compile_options(patchcoretest PRIVATE -Wall -Ofast -fno-finite-math-only)