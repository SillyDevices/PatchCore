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

#ifndef DIGITRON_INDICATORMODULE_HPP
#define DIGITRON_INDICATORMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include <map>

#define INDICATOR_INPUT "in"

class IndicatorModule: public Module {
public:
    IndicatorModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameter);
    IndicatorModule(std::string name, int sampleRate);
    IndicatorModule(const IndicatorModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~IndicatorModule() = default;

    void envelope() override;

public:

    void setBufferSize(int size);
    // write to plain buffer, returns the number of samples written( <= destSize)
    int copyIntoBuffer(float* dest, int destSize, int startIndex);
    int getAvailable();

public:
    bool needEnvelopeOnInputConnection() const override;

public:
    ModuleInput input = ModuleInput(INDICATOR_INPUT);

protected:
    int size;
    std::vector<float> ringBuffer;
    int readIndex = 0;
    int writeIndex = 0;
    bool overflow = false;
};

#endif //DIGITRON_INDICATORMODULE_HPP
