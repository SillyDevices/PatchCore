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

#include "patchcore/module/input/module/TriggerModuleInput.hpp"


TriggerModuleInput::TriggerModuleInput(std::string name, TriggerModuleInput::OnTriggerCallback *callback):
ModuleInput(name), callback(callback) { }

void TriggerModuleInput::envelope() {
    if (isOn && value < TRIGGER_OFF_THRESHOLD) {
        isOn = false;
    } else if (!isOn && value > TRIGGER_ON_THRESHOLD) {
        isOn = true;
        callback->onTrigger();
    }
}


