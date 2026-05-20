#ifndef PATCHCORE_TEST_BLOCK_UTILS_HPP
#define PATCHCORE_TEST_BLOCK_UTILS_HPP

#include <patchcore/synth/ModularSynth.hpp>

#include <algorithm>
#include <utility>

namespace patchcore_test {

template <typename Callback>
void computeSynthSamples(ModularSynth* synth, int sampleCount, Callback callback) {
    StereoBlock block;
    int processed = 0;
    while (processed < sampleCount) {
        synth->computeBlock(block);
        const int framesToRead = std::min(sampleCount - processed, PATCHCORE_BLOCK_SIZE);
        for (int sampleIndex = 0; sampleIndex < framesToRead; ++sampleIndex) {
            callback(processed + sampleIndex, std::pair<float, float>{
                    block.left[sampleIndex],
                    block.right[sampleIndex]
            });
        }
        processed += framesToRead;
    }
}

inline std::pair<float, float> computeLastSynthSample(ModularSynth* synth, int sampleCount) {
    std::pair<float, float> lastResult = {0.0f, 0.0f};
    computeSynthSamples(synth, sampleCount, [&](int, std::pair<float, float> result) {
        lastResult = result;
    });
    return lastResult;
}

} // namespace patchcore_test

#endif //PATCHCORE_TEST_BLOCK_UTILS_HPP
