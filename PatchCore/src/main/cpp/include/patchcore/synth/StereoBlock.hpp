#ifndef PATCHCORE_STEREO_BLOCK_H
#define PATCHCORE_STEREO_BLOCK_H

#include "patchcore/module/buffer/BlockConfig.hpp"

#include <array>

struct StereoBlock {
    std::array<float, PATCHCORE_BLOCK_SIZE> left {};
    std::array<float, PATCHCORE_BLOCK_SIZE> right {};
};

#endif //PATCHCORE_STEREO_BLOCK_H
