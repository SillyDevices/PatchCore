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

#ifndef PATCHCORE_LEDOUTPUT_HPP
#define PATCHCORE_LEDOUTPUT_HPP

#include "patchcore/module/output/Output.hpp"
#include "patchcore/module/output/ModuleOutput.hpp"
#include <string>
#include <vector>


class LedOutput: public ModuleOutput {

public:
    LedOutput(std::string outputName, int sampleRate): ModuleOutput(outputName), buffer(std::vector<float>(sampleRate/50)) {
        for (int i = 0; i < buffer.size(); i++) {
            buffer[i] = 0;
        }
    };
    virtual ~LedOutput() = default;

    float smoothingFactor = 0.0085f;

    std::vector<float> buffer;
    int writePointer = 0;

//    inline void setValue(float newValue) override {
//        buffer[writePointer] = newValue;
//        writePointer = (writePointer + 1) % buffer.size();
//    };

    //TODO call this!
    inline void envelope() {
        buffer[writePointer] = value;
        writePointer = (writePointer + 1) % buffer.size();
    }

    inline float getLedValue() {
        auto sum = 0.0f;
        for (auto value: buffer) {
            sum += value;
        }
        return sum / buffer.size();
    }

};

#endif //PATCHCORE_LEDOUTPUT_HPP
