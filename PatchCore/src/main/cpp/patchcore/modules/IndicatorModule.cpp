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
#include <stdexcept>
#include <android/log_macros.h>

#undef LOG_TAG
#define LOG_TAG "IndicatorModule"


IndicatorModule::IndicatorModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameter)
        :IndicatorModule(name, sampleRate) {}


IndicatorModule::IndicatorModule(std::string name, int sampleRate)
    : Module(name, sampleRate), size(100), overflow(false) {
    init();
}

IndicatorModule::IndicatorModule(const IndicatorModule& other)
    : Module(other.name, other.sampleRate), size(other.size), overflow(other.overflow) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> IndicatorModule::clone() const {
    throw std::runtime_error("IndicatorModule::clone() is not implemented yet");
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
    bufferPtr[writeIndex] = input.value;
    if (writeIndex == 3) {
        ALOGD("cpp buffer [0] = %f, [1] = %f, [2] = %f",
               bufferPtr[0], bufferPtr[1], bufferPtr[2]);
    }
    writeIndex = (writeIndex + 1) % size;
}

void IndicatorModule::setBuffer(float *buffer, int newSize) {
    this->bufferPtr = buffer;
    this->size = newSize;
    this->writeIndex = 0;
    this->overflow = false;
}

bool IndicatorModule::needEnvelopeOnInputConnection() const {
    return true;
}