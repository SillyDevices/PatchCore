#ifndef PATCHCORE_BLOCKCONTEXT_HPP
#define PATCHCORE_BLOCKCONTEXT_HPP

struct BlockContext {
    int blockSize = 0;
    int sampleRate = 0;
    long long blockStartSample = 0;
    double blockStartTimeUs = 0.0;
};

#endif //PATCHCORE_BLOCKCONTEXT_HPP
