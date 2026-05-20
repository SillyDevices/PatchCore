#ifndef PATCHCORE_BLOCKCONTEXT_HPP
#define PATCHCORE_BLOCKCONTEXT_HPP

#include <cstdint>

struct BlockContext {
    int blockSize = 0;
    int sampleRate = 0;
    int64_t blockStartSample = 0;
    double blockStartTimeUs = 0.0;
};

#endif //PATCHCORE_BLOCKCONTEXT_HPP
