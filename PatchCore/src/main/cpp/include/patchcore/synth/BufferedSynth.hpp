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

#ifndef PATCHCORE_BUFFERED_SYNTH_H
#define PATCHCORE_BUFFERED_SYNTH_H

#include "patchcore/synth/AbstractSynth.hpp"

#include <vector>

//namespace PatchCore {

class BufferedSynth: public AbstractSynth {
public:
    virtual ~BufferedSynth() {} ;
public:
    virtual void computeSamples(int count);

    virtual std::vector<float>* getLeftChannelSamples();
    virtual std::vector<float>* getRightChannelSamples();

protected:
    int _bufferSize = 0;
    std::vector<float> _leftOutputBuffer;
    std::vector<float> _rightOutputBuffer;
};

//}

#endif //PATCHCORE_BUFFERED_SYNTH_H
