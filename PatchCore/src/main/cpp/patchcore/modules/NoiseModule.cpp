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

#include "patchcore/modules/NoiseModule.hpp"

NoiseModule::NoiseModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : NoiseModule(name, sampleRate) {
}

NoiseModule::NoiseModule(std::string name, int sampleRate)
    : Module(name, sampleRate), noise(), output(NOISEMODULE_OUTPUT), pinkOutput(NOISEMODULE_PINK_OUTPUT) {
    init();
}

NoiseModule::NoiseModule(const NoiseModule& other)
    : Module(other.name, other.sampleRate), noise(other.noise), output(NOISEMODULE_OUTPUT), pinkOutput(NOISEMODULE_PINK_OUTPUT) {
    init();
    b0 = other.b0; b1 = other.b1; b2 = other.b2; b3 = other.b3;
    b4 = other.b4; b5 = other.b5; b6 = other.b6;
    pinkBuffer = other.pinkBuffer;
    copyIOs(other);
}

[[nodiscard]] std::unique_ptr<Module> NoiseModule::clone() const {
    return std::make_unique<NoiseModule>(*this);
}

void NoiseModule::init() {
    registerOutput(output);
    registerOutput(pinkOutput);
}

void NoiseModule::envelope() {

    float white = noise.envelope();
    pinkBuffer.push_front(white);
    pinkBuffer.pop_back();
    auto delayed = *(pinkBuffer.begin());


    b0 = 0.99886f * b0 + delayed * 0.0555179f;
    b1 = 0.99332f * b1 + delayed * 0.0750759f;
    b2 = 0.96900f * b2 + delayed * 0.1538520f;
    b3 = 0.86650f * b3 + delayed * 0.3104856f;
    b4 = 0.55000f * b4 + delayed * 0.5329522f;
    b5 = -0.7616f * b5 - delayed * 0.0168980f;
    float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + delayed * 0.5362f;
    b6 = delayed * 0.115926f;

    output.value = white;
    pinkOutput.value = pink * 0.15;
}
