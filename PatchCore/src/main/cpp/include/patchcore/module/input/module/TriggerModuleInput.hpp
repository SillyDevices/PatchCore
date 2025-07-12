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

#ifndef PATCHCORE_TRIGGERMODULEINPUT_HPP
#define PATCHCORE_TRIGGERMODULEINPUT_HPP


#include "patchcore/module/input/ModuleInput.hpp"
#include <string>

#define TRIGGER_ON_THRESHOLD 0.5f
#define TRIGGER_OFF_THRESHOLD 0.4f

class TriggerModuleInput: public ModuleInput{
public:
    class OnTriggerCallback {
    public:
        virtual ~OnTriggerCallback() = default;
        virtual void onTrigger() = 0;
    };
public:
    TriggerModuleInput(std::string name, OnTriggerCallback *callback);
    virtual ~TriggerModuleInput() = default;
public:
    void envelope();
private:
    OnTriggerCallback *callback;
    bool isOn = false;
};

#endif //PATCHCORE_TRIGGERMODULEINPUT_HPP
