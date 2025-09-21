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

#include "patchcore/modules/input/event/keyboard/poly/PolyProxyKeyboardModule.hpp"
#include "patchcore/module/PolyModule.hpp"

PolyProxyKeyboardModule::PolyProxyKeyboardModule(const Module *module, PolyModule *polyModule):
PolyProxyModule(module, polyModule), SingleKeyboardModule(module->getModuleName(), module->getSampleRate()) {
    this->polyModule = polyModule;
    keyboards.reserve(modulesToProxy.size());
    for (Module* _keyboard : modulesToProxy) {
        if (auto casted = dynamic_cast<SingleKeyboardModule*>(_keyboard)) {
            keyboards.push_back(casted);
        } else {
            throw std::runtime_error("PolyProxyKeyboardModule can only proxy SingleKeyboardModule instances");
        }
    }
    keyboard = std::make_unique<RouterKeyboard>(module->getModuleName(), module->getSampleRate(), keyboards);

}

void PolyProxyKeyboardModule::onEvent(KeyboardEvent event) {
    //TODO optimize. We don't need to set polyphony on every event.
    keyboard->setPolyphony(polyModule->getActiveVoiceCount());
    keyboard->onEvent(event);
}