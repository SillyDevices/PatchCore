#include "PatchCoreLogger.hpp"

#ifdef __ANDROID__
#include <android/log.h>
#else
#include <cstdio>
#endif

void patchcoreLogInfo(const char* tag, const std::string& message) {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, tag, "%s", message.c_str());
#else
    (void) tag;
    std::printf("%s\n", message.c_str());
#endif
}
