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

#ifndef PATCHCORE_PROXYMODULEUSERINPUT_HPP
#define PATCHCORE_PROXYMODULEUSERINPUT_HPP

#include "patchcore/module/input/UserInput.hpp"

// Note: Descendants of this class should inherit from UserInput
class ProxyModuleUserInput {
public:
    ProxyModuleUserInput(UserInput* userInput):  _userInput(userInput) {

    };
    virtual ~ProxyModuleUserInput() = default;

public:
    [[nodiscard]]
    virtual UserInput* getUserInput() const {
        return _userInput;
    };

private:
    UserInput* _userInput;
};

#endif //PATCHCORE_PROXYMODULEUSERINPUT_HPP
