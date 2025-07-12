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
#include <gtest/gtest.h>
#include <android/log.h>

class AndroidTestLogger : public ::testing::EmptyTestEventListener {
public:
    void OnTestStart(const ::testing::TestInfo& test_info) override {
        __android_log_print(ANDROID_LOG_INFO, "NativeTest", "Starting test: %s.%s",
                            test_info.test_case_name(), test_info.name());
    }
    void OnTestPartResult(const ::testing::TestPartResult& result) override {
        if (result.failed()) {
            __android_log_print(ANDROID_LOG_ERROR, "NativeTest",
                                "Failure in %s:%d\n%s",
                                result.file_name(), result.line_number(), result.summary());
        }
    }
    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        if (test_info.result()->Passed()) {
            __android_log_print(ANDROID_LOG_INFO, "NativeTest",
                                "Test passed: %s.%s",
                                test_info.test_case_name(), test_info.name());
        } else {
            __android_log_print(ANDROID_LOG_ERROR, "NativeTest",
                                "Test failed: %s.%s",
                                test_info.test_case_name(), test_info.name());
        }
    }
};

extern "C"
JNIEXPORT jint JNICALL
Java_com_sillydevices_patchcore_test_NativeInstrumentedTest_runNativeTests(JNIEnv *env, jobject thiz) {
    ::testing::InitGoogleTest();
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new AndroidTestLogger());
    return RUN_ALL_TESTS();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_sillydevices_patchcore_test_NativeInstrumentedTest_runSingleNativeTest(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jstring test_name) {
    auto chars = env->GetStringUTFChars(test_name, nullptr);
    std::string testName(chars);
    ::testing::InitGoogleTest();
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new AndroidTestLogger());
    ::testing::GTEST_FLAG(filter) = testName;
    auto result = RUN_ALL_TESTS();
    env->ReleaseStringUTFChars(test_name, chars);
    return result;
}