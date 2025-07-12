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

#ifndef PATCHCORE_ABSTRACTROUTERKEYBOARD_HPP
#define PATCHCORE_ABSTRACTROUTERKEYBOARD_HPP


#include "patchcore/module/input/user/special/CallbackBooleanUserInput.hpp"
#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include <map>
#include <vector>
#include <list>
#include <mutex>


//TODO remove Module interface, use this class as delegate for KeyboardModule

class AbstractRouterKeyboard: public EventKeyboard, public Module {
private:
    class MultiTriggerCallback: public CallbackBooleanUserInput::OnSetCallback {
    public:
        MultiTriggerCallback(AbstractRouterKeyboard *module): module(module) {}
        virtual ~MultiTriggerCallback() = default;
        void onSet(bool value) override {
            module->onMultiTrigger(value);
        }
    private:
        AbstractRouterKeyboard *module;
    };

public:
    AbstractRouterKeyboard(std::string name, int sampleRate, std::vector<KeyboardModule*> keyboards);
    virtual ~AbstractRouterKeyboard() = default;

    void envelope() override;

    virtual void setPolyphony(int polyphony);
    virtual void onEvent(KeyboardEvent event) override = 0;

private:
    MultiTriggerCallback multiTriggerCallback = MultiTriggerCallback(this);
    virtual void onMultiTrigger(bool value);
public:
    CallbackBooleanUserInput multiTriggerInput = CallbackBooleanUserInput(KEYBOARD_INPUT_MULTI_TRIGGER, &multiTriggerCallback);
protected:
    std::vector<KeyboardModule*> _keyboards;
    int _size = 0;
    int _polyphony = 0;

    std::mutex _mutex;
};

#endif //PATCHCORE_ABSTRACTROUTERKEYBOARD_HPP
