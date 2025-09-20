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

#ifndef PATCHCORE_POLYPROXYKEYBOARDMODULE_HPP
#define PATCHCORE_POLYPROXYKEYBOARDMODULE_HPP

#include "patchcore/module/PolyProxyModule.hpp"
#include "patchcore/modules/input/event/keyboard/SingleKeyboardModule.hpp"
#include "patchcore/modules/input/event/keyboard/poly/RouterKeyboard.hpp"

#include <map>
#include <list>


class PolyProxyKeyboardModule : public PolyProxyModule, public SingleKeyboardModule {
public:
    PolyProxyKeyboardModule(const Module *module, PolyModule *polyModule);

public:
    void onStartBuffer(int size) override {
        throw std::runtime_error("PolyProxyKeyboardModule does not implement envelope method");
    }
    void envelope() override {
        throw std::runtime_error("PolyProxyKeyboardModule does not implement envelope method");
    }
public:
    void onEvent(KeyboardEvent event) override;

protected:
    PolyModule *polyModule;
    std::unique_ptr<RouterKeyboard> keyboard;
    std::vector<KeyboardModule *> keyboards;

};

#endif //PATCHCORE_POLYPROXYKEYBOARDMODULE_HPP
