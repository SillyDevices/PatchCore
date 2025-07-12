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

#ifndef PATCHCORE_KEYBOARDMODULE_HPP
#define PATCHCORE_KEYBOARDMODULE_HPP


#include "patchcore/modules/input/AbstractKeyboard.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"
#include "patchcore/module/input/user/special/CallbackFloatUserInput.hpp"
#include "patchcore/module/input/user/special/OnStartBufferCallbackUserInput.hpp"
#include "patchcore/module/input/user/special/PassFloatUserInput.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"
#include "patchcore/modules/input/event/keyboard/EventKeyboard.hpp"
#include <mutex>

class KeyboardModule: public AbstractKeyboard, public EventKeyboard {
private:
    struct NoteChange{
        long sample;
        bool isOn;
        float cv;
        float velocity;
    };
protected:
    struct Key {
        float cv;
        float velocity;
    };

public:
    KeyboardModule(std::string name, int sampleRate);
    KeyboardModule(const KeyboardModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~KeyboardModule() = default;

public:
    void onStartBuffer(int size) override;
    void envelope() override;
public:
    void onEvent(KeyboardEvent event) override;

public:
    OnStartBufferCallbackUserInput callbackInput = OnStartBufferCallbackUserInput("KeyboardModuleCallback", this);
    BoolUserInput multiTriggerInput = BoolUserInput(KEYBOARD_INPUT_MULTI_TRIGGER);

private:
    void onNoteOn(float cv, float velocity);
    void onNoteOff(float cv, float velocity);

protected:
    std::vector<Key> pressedKeys;

protected:
    float _lastCv = .0f;
    std::vector<float> _onNotes = std::vector<float>();

protected:
    double syncTime = -1;
    int syncOffset = 0;
    const double sampleTime = 1000000.0 / AbstractKeyboard::sampleRate;

    std::vector<NoteChange*> noteChanges = std::vector<NoteChange*>();

    std::mutex changesMutex;

};



#endif //PATCHCORE_KEYBOARDMODULE_HPP
