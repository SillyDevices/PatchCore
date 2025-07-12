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

#ifndef PATCHCORE_CALLBACKBOOLEANUSERINPUT_HPP
#define PATCHCORE_CALLBACKBOOLEANUSERINPUT_HPP

#include "patchcore/module/input/user/BoolUserInput.hpp"
#include "patchcore/callback/OnStartBuffer.hpp"
#include <string>

class CallbackBooleanUserInput: public BoolUserInput{
public:
    class OnSetCallback {
    public:
        virtual ~OnSetCallback() = default;
        virtual void onSet(bool newValue) = 0;
    };
public:
    CallbackBooleanUserInput(std::string name, OnSetCallback *callback);
    //TODO check for memory leak with this destructor
    virtual ~CallbackBooleanUserInput() = default;
public:
    inline void setValue(bool newValue);
private:
    OnSetCallback *callback;
};

inline void CallbackBooleanUserInput::setValue(bool newValue) {
    callback->onSet(newValue);
}

#endif //PATCHCORE_CALLBACKBOOLEANUSERINPUT_HPP
