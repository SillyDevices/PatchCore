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

#include "patchcore/modules/input/event/keyboard/poly/RouterKeyboard.hpp"
#include <algorithm>


RouterKeyboard::RouterKeyboard(std::string name, int sampleRate, std::vector<KeyboardModule*> keyboards):
        AbstractRouterKeyboard(name, sampleRate, keyboards){
}

void RouterKeyboard::onEvent(KeyboardEvent event) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (event.press) {
        //if keyboard is already pressed, do nothing
        if (_keyboardsMap.find(event.cv) != _keyboardsMap.end()) {
            return;
        }
        auto unUsedKeyboards = std::list<KeyboardModule *>(_polyphony);
        std::copy_n(_keyboards.begin(), _polyphony, unUsedKeyboards.begin());
        for (auto usedKeyboard: _usedKeyboards) {
            unUsedKeyboards.remove(usedKeyboard);
        }
        KeyboardModule* keyboard;
        if (unUsedKeyboards.empty()){
            keyboard = _usedKeyboards.front();
            _usedKeyboards.pop_front();
        } else {
            keyboard = unUsedKeyboards.front();
        }
        keyboard->onEvent(event);
        _usedKeyboards.push_back(keyboard);
        _keyboardsMap[event.cv] = keyboard;
    } else {
        auto keyboard = _keyboardsMap[event.cv];
        if (keyboard) {
            keyboard->onEvent(event);
        }
        _keyboardsMap.erase(event.cv);
        _usedKeyboards.remove(keyboard);
    }
}

void RouterKeyboard::setPolyphony(int count) {
    std::lock_guard<std::mutex> lock(_mutex);
    AbstractRouterKeyboard::setPolyphony(count);

    for (auto pair: _keyboardsMap) {
        pair.second->onEvent(KeyboardEvent {
            .press = false,
            .cv = pair.first,
            .velocity = 0.0f,
        } );
    }
    _keyboardsMap.clear();
    _usedKeyboards.clear();

//    //remove keyboard if polyphony is lower than used keyboards
//    auto diff = _usedKeyboards.size() - _polyphony;
//    if (diff > 0) {
//        for (int i = 0; i < diff; i++) {
//            auto keyboard = _usedKeyboards.front();
//            if (keyboard == nullptr) {
//                continue;
//            }
//            //find keyboard in map
//            auto it = std::find_if(_keyboardsMap.begin(), _keyboardsMap.end(), [keyboard](const std::pair<float, NewKeyboardModule*>& element) {
//                return element.second == keyboard;
//            });
//            if (it != _keyboardsMap.end()) {
//                auto note = it->first;
//                _keyboardsMap.erase(it);
//                keyboard->noteOff.setValue(note);
//            }
//            _usedKeyboards.pop_front();
//        }
//    }
}



