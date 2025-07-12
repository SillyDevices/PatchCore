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

#ifndef PATCHCORE_ROUTERKEYBOARD_HPP
#define PATCHCORE_ROUTERKEYBOARD_HPP

#include "AbstractRouterKeyboard.hpp"
#include "patchcore/module/input/user/special/CallbackBooleanUserInput.hpp"
#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include <map>
#include <vector>
#include <list>
#include <mutex>


// used as proxy, route polyphonic keyboard to multiple monophonic keyboards
class RouterKeyboard: public AbstractRouterKeyboard {
public:
    RouterKeyboard(std::string name, int sampleRate, std::vector<KeyboardModule*> keyboards);
    virtual ~RouterKeyboard() = default;

    void setPolyphony(int polyphony) override;
    void onEvent(KeyboardEvent event) override;

    std::unique_ptr<Module> clone() const override {
        throw std::runtime_error("RouterKeyboard does not implement clone method");
    }
private:
    std::map<float, KeyboardModule*> _keyboardsMap;
    std::list<KeyboardModule*> _usedKeyboards;
};

#endif //PATCHCORE_ROUTERKEYBOARD_HPP
