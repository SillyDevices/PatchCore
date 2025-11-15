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
#include <atomic>
#include <android/log_macros.h>

#undef LOG_TAG
#define LOG_TAG "IndicatorModule"



IndicatorModule::IndicatorModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameter)
        :IndicatorModule(name, sampleRate) {}


IndicatorModule::IndicatorModule(std::string name, int sampleRate)
    : Module(name, sampleRate), overflow(false) {

    size = 104;
    dataSize = size - 4;
    bufferPtr = new float[size];
    dataPtr = bufferPtr + 4;

    writeIndex = 0;
    overflow = false;
    init();
}

IndicatorModule::IndicatorModule(const IndicatorModule& other)
    : Module(other.name, other.sampleRate), size(other.size), overflow(other.overflow) {
    init();
    copyIOs(other);
}

std::unique_ptr<Module> IndicatorModule::clone() const {
    throw std::runtime_error("IndicatorModule::clone() Cloning is not supported for IndicatorModule");
}

void IndicatorModule::init() {
    registerInput(input);
}

std::unique_ptr<PolyProxyModule> IndicatorModule::createPolyModuleProxy(PolyModule *polyModule) const {
    throw std::runtime_error("IndicatorModule::createPolyModuleProxy() This module is not intended for polyphonic use");
}

void IndicatorModule::envelope() {
    dataPtr[writeIndex] = input.value;
    writeIndex = (writeIndex + 1) % (dataSize);
}

void IndicatorModule::onStartBuffer(int bufferSize) {
    Module::onStartBuffer(bufferSize);

    //swap to new buffer without blocking audio thread
    if (targetBufferDescriptor != nullptr) {
        //TODO create global buffer allocator, and here only mark old buffer as free
        delete[] bufferPtr;

        size = targetBufferDescriptor->size;
        dataSize = targetBufferDescriptor->dataSize;
        bufferPtr = targetBufferDescriptor->bufferPtr;
        dataPtr = targetBufferDescriptor->dataPtr;
        delete targetBufferDescriptor;
        targetBufferDescriptor = nullptr;

        writeIndex = 0;
        overflow = false;
    }

    uint32_t v = writeIndex;
    std::atomic_thread_fence(std::memory_order_release);
    // write writeIndex to header at start of buffer
    std::memcpy(bufferPtr, &v, sizeof(v));


}

float* IndicatorModule::getBuffer(int requestedDataSize) {
    std::lock_guard<std::mutex> lock(getBufferMutex);
    if (size == requestedDataSize && targetBufferDescriptor == nullptr) {
        // requested size matches current buffer size, no need to allocate new buffer
        return bufferPtr;
    }
    if (targetBufferDescriptor != nullptr && targetBufferDescriptor->dataSize == requestedDataSize) {
        // requested size matches already allocated target buffer size
        return targetBufferDescriptor->bufferPtr;
    }

    // allocate new buffer
    //TODO create global buffer allocator
    float* buf = new float[requestedDataSize + 4];
    targetBufferDescriptor = new BufferDescriptor();
    targetBufferDescriptor->size = requestedDataSize + 4;
    targetBufferDescriptor->dataSize = requestedDataSize;
    targetBufferDescriptor->bufferPtr = buf;
    targetBufferDescriptor->dataPtr = buf + 4;

    // prepare buffer, set writeIndex to 0
    uint32_t v = 0;
    std::memcpy(targetBufferDescriptor->bufferPtr, &v, sizeof(v));
    std::memset(targetBufferDescriptor->dataPtr, 0, targetBufferDescriptor->dataSize * sizeof(float));

    // return new buffer, will be used on next buffer cycle
    return buf;
}

bool IndicatorModule::needEnvelopeOnInputConnection() const {
    return true;
}