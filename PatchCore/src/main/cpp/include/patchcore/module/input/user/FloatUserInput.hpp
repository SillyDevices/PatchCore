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

#ifndef PATCHCORE_FLOATUSERINPUT_HPP
#define PATCHCORE_FLOATUSERINPUT_HPP

#include "patchcore/module/input/UserInput.hpp"
#include "patchcore/callback/OnStartBuffer.hpp"
#include <string>


class FloatUserInput: public UserInput, public OnStartBuffer {
public:
    explicit FloatUserInput(std::string name);
    FloatUserInput(std::string name, float speed);
    virtual ~FloatUserInput() = default;
public:

    virtual inline void setValue(float newValue);

    virtual inline void setParameterLockValue(float newValue);
    virtual void clearParameterLock();

    void onStartBuffer(int bufferSize) override;

    virtual void envelope();

public:
    void copyFrom(UserInput &other) {
        if (other.type != UserInputType::FLOAT) {
            throw std::runtime_error("Cannot copy from non-bool UserInput");
        }
        auto castedOther = dynamic_cast<FloatUserInput&>(other);
        lastSetValue = castedOther.lastSetValue;
        lastSetParameterLockValue = castedOther.lastSetParameterLockValue;
        startValue = castedOther.startValue;
        targetValue = castedOther.targetValue;
        delta = castedOther.delta;
        speed = castedOther.speed;
        value = castedOther.value;
    }

    float getSpeed() const {
        return speed;
    }

    virtual ProxyModuleUserInput* createProxy(const std::string &withName) override;
    virtual std::unique_ptr<PolyProxyUserInput> createPolyProxy(std::vector<Module *> modulesToProxy) override;

protected:
    //TODO add to constructor
    int sampleRate = 44100;

    float lastSetValue = .0f;
    float lastSetParameterLockValue = .0f;
    bool isLocked = false;

    float startValue = .0f;
    float targetValue = .0f;

    float delta = .0f;

    float speed = 5.0f;
public:
    float value = .0f;
};

inline void FloatUserInput::setValue(float newValue) {
    lastSetValue = newValue;
    isLocked = false;
}

inline void FloatUserInput::setParameterLockValue(float newValue) {
    value = newValue;
    lastSetParameterLockValue = newValue;
    isLocked = true;
}

#endif //PATCHCORE_FLOATUSERINPUT_HPP
