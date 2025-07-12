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

#include "patchcore/modules/DelayLineModule.hpp"
#include "patchcore/dsp/dsp.h"

DelayLineModule::DelayLineModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : DelayLineModule(name, sampleRate,
                      ModuleParameter::getFloatOrDefault(parameters, DELAYLINE_MODULE_PARAMETER_MAX_TIME, 3.0f),
                      (InterpolationType) ModuleParameter::getIntOrDefault(parameters, DELAYLINE_MODULE_PARAMETER_INTERPOLATION, QUADRATIC)
                ) {
}

DelayLineModule::DelayLineModule(std::string name, int sampleRate, float maxTime, InterpolationType interpolationType)
    : Module(name, sampleRate), type(interpolationType), maxTime(maxTime) {
    init();
    try {
        bufferSize = (int)(maxTime * (float)sampleRate) + 1;
        buffer = new float[bufferSize];
        for (int i = 0; i < bufferSize; i++) {
            buffer[i] = 0;
        }
    } catch (const std::bad_alloc& e) {
        bufferSize = 1;
        buffer = nullptr;
    }
}

DelayLineModule::DelayLineModule(const DelayLineModule& other)
    : Module(other.name, other.sampleRate), type(other.type), maxTime(other.maxTime) {
    init();
    try {
        bufferSize = (int)(maxTime * (float)sampleRate) + 1;
        buffer = new float[bufferSize];
        for (int i = 0; i < bufferSize; i++) {
            buffer[i] = 0;
        }
        writePointer = -1;
    } catch (const std::bad_alloc& e) {
        bufferSize = 1;
        buffer = nullptr;
        writePointer = other.writePointer;
    }
    copyIOs(other);
}

std::unique_ptr<Module> DelayLineModule::clone() const {
    return std::make_unique<DelayLineModule>(*this);
}

void DelayLineModule::init() {
    registerInput(input);
    registerInput(inputTime);
    registerOutput(output);
    registerUserInput(userInputTime);
}

DelayLineModule::~DelayLineModule() {
    delete[] buffer;
}

//I have confidence in this implementation
void DelayLineModule::envelope() {
    if (buffer == nullptr) {
        output.value = 0;
        return;
    }

    writePointer = (writePointer + 1) % bufferSize;
//    writePointer++;
//    if (writePointer >= bufferSize) {
//        writePointer = 0;
//    }
    buffer[writePointer] = input.value;

    // 0 < < bufferSize
    float deltaSamples = std::min(std::max( (inputTime.value + userInputTime.value) * ( (float)sampleRate ), 0.0f), (float) bufferSize);

    float readPointer = (float) writePointer - deltaSamples + (float) bufferSize;
    if (readPointer >= (float) bufferSize) {
        readPointer -= (float) bufferSize;
    }

    int intReadPointer = (int)floorf(readPointer);
    int actualReadPointer = (intReadPointer + bufferSize)%(bufferSize);

    float result = 0;
    if (type == NONE) {
        result = buffer[actualReadPointer];
    } else if (type == LINEAR) {
        float fraction = readPointer - (float) intReadPointer;
        int nextReadPointer = intReadPointer + 1;
        if (nextReadPointer >= bufferSize) { nextReadPointer = 0; }
        result = dsp::linearInterpolation(buffer[actualReadPointer], buffer[nextReadPointer], fraction);
    } else if (type == QUADRATIC) {
        float fraction = readPointer - (float) intReadPointer;
        int prevReadPointer = intReadPointer - 1;
        if (prevReadPointer < 0) { prevReadPointer = bufferSize - 1; }
        int nextReadPointer = intReadPointer + 1;
        if (nextReadPointer >= bufferSize) { nextReadPointer = 0; }
        result = dsp::quadraticInterpolation(buffer[prevReadPointer], buffer[actualReadPointer],buffer[nextReadPointer], fraction);
    } else if (type == CUBIC) {
        float fraction = readPointer - (float) intReadPointer;
        int prevReadPointer = intReadPointer - 1;
        if (prevReadPointer < 0) { prevReadPointer = bufferSize - 1; }
        int nextReadPointer = intReadPointer + 1;
        if (nextReadPointer >= bufferSize) { nextReadPointer = 0; }
        int nextNextReadPointer = nextReadPointer + 1;
        if (nextNextReadPointer >= bufferSize) { nextNextReadPointer = 0; }
        result = dsp::cubicInterpolation(buffer[prevReadPointer], buffer[actualReadPointer],buffer[nextReadPointer], buffer[nextNextReadPointer], fraction);
    }
    output.value = result;
}
