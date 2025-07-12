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

#ifndef PATCHCORE_SINGLEKEYBOARDMODULE_HPP
#define PATCHCORE_SINGLEKEYBOARDMODULE_HPP


#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"

class SingleKeyboardModule: public KeyboardModule {
public:
    SingleKeyboardModule(std::string name, int sampleRate);
    SingleKeyboardModule(const SingleKeyboardModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~SingleKeyboardModule() = default;

    std::unique_ptr<PolyProxyModule> createPolyModuleProxy(PolyModule *polyModule) const override;

public:
    void envelope() override;
private:
    float _cv = 0.0f;
    float _velocity = 0.5f;
    bool _gate = false;
};



#endif //PATCHCORE_SINGLEKEYBOARDMODULE_HPP
