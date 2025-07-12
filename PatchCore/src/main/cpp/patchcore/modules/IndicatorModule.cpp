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

#include "patchcore/modules/IndicatorModule.hpp"


IndicatorModule::IndicatorModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameter)
        :IndicatorModule(name, sampleRate) {}


IndicatorModule::IndicatorModule(std::string name, int sampleRate)
    : Module(name, sampleRate), size(5 * sampleRate / 30), overflow(false) {
    ringBuffer = std::vector<float>(size);
    init();
}

IndicatorModule::IndicatorModule(const IndicatorModule& other)
    : Module(other.name, other.sampleRate), size(other.size), overflow(other.overflow) {
    ringBuffer = std::vector<float>(other.size);
    init();
    copyIOs(other);
}

std::unique_ptr<Module> IndicatorModule::clone() const {
    return std::make_unique<IndicatorModule>(*this);
}

void IndicatorModule::init() {
    registerInput(input);
}



int getReadSize(int readIndex, int writeIndex, int size, bool overflow){
    if (overflow) return size;
    if (writeIndex > readIndex){
        //from readIndex to writeIndex
        return writeIndex - readIndex;
    } else if ( writeIndex < readIndex) {
        // from readIndex to the end of the buffer and from the beginning to writeIndex
        return size - readIndex + writeIndex;
    }
    return 0;
}


void IndicatorModule::envelope() {
    ringBuffer[writeIndex] = input.value;
    writeIndex = (writeIndex + 1) % size;
    if (writeIndex == readIndex) {
        overflow = true;
    }
}

void IndicatorModule::setBufferSize(int newSize) {
    if (newSize <= 0) return;
    if (ringBuffer.size() != newSize) {
        ringBuffer.resize(newSize, 0.0f);
        writeIndex = 0;
        readIndex = 0;
    }
}

int IndicatorModule::copyIntoBuffer(float *dest, int destSize, int startIndex) {
    int currentWriteIndex = writeIndex;
    if (overflow) {
        //readIndex = writeIndex;
    }
    int available = getReadSize(readIndex, currentWriteIndex, size, overflow);
    overflow = false;
    int samplesToCopy =
            available > destSize - startIndex ?
            destSize - startIndex :
            available;
    if (samplesToCopy > 0){
        if (readIndex + samplesToCopy <= size){
            memcpy(dest + startIndex, &ringBuffer[readIndex], samplesToCopy * sizeof(float));
        } else {
            auto firstChunkSize = size - readIndex;
            memcpy(dest + startIndex, &ringBuffer[readIndex], firstChunkSize * sizeof(float));
            memcpy(dest + startIndex + firstChunkSize, &ringBuffer[0], (samplesToCopy - firstChunkSize) * sizeof(float));
        }
        readIndex = (readIndex + samplesToCopy) % size;
        return samplesToCopy;
    }
    return 0;
}

int IndicatorModule::getAvailable() {
    int currentWriteIndex = writeIndex;
    if (overflow) {
        readIndex = writeIndex;
    }
    int available = getReadSize(readIndex, currentWriteIndex, size, overflow);
    return available;
}

bool IndicatorModule::needEnvelopeOnInputConnection() const {
    return true;
}