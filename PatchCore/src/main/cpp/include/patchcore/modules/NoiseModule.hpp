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

#ifndef PATCHCORE_NOISEMODULE_HPP
#define PATCHCORE_NOISEMODULE_HPP


#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/dsp/osc/Noise.hpp"
#include <array>

#define NOISEMODULE_OUTPUT "out"
#define NOISEMODULE_PINK_OUTPUT "pink_out"

class NoiseModule: public Module{
public:
    NoiseModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    NoiseModule(std::string name, int sampleRate);
    NoiseModule(const NoiseModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    void init();
    ~NoiseModule() override = default;

public:
    void processSample(int sampleIndex) override;
private:
    Noise noise = Noise();
    ModuleOutput output = ModuleOutput(NOISEMODULE_OUTPUT);
    ModuleOutput pinkOutput = ModuleOutput(NOISEMODULE_PINK_OUTPUT);
private:
    static constexpr int pinkBufferSize = 10;
    float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;
    std::array<float, pinkBufferSize> pinkBuffer = {};
    int pinkBufferWriteIndex = 0;
};

#endif //PATCHCORE_NOISEMODULE_HPP
