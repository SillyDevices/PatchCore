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


#ifndef PATCHCORE_BASEENVELOPE_HPP
#define PATCHCORE_BASEENVELOPE_HPP

#include "patchcore/module/Module.hpp"

#define ENVELOPE_GATE_ON_THRESHOLD 0.5f
#define ENVELOPE_GATE_OFF_THRESHOLD 0.4f

#define ENVELOPE_INPUT_GATE "gate"

#define ENVELOPE_OUTPUT "out"


class BaseEnvelope: public Module {
public:
    BaseEnvelope(std::string name, int sampleRate);

    virtual ~BaseEnvelope() = default;

    void envelope() override;

protected:
    virtual void internalEnvelope(bool gate) = 0;
protected:
    ModuleInput inputGate = ModuleInput(ENVELOPE_INPUT_GATE);
    ModuleOutput output = ModuleOutput(ENVELOPE_OUTPUT);
protected:
    bool lastGate = false;
};


#endif //PATCHCORE_BASEENVELOPE_HPP
