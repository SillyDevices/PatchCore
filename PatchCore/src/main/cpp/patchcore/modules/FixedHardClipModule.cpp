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

#include "patchcore/modules/FixedHardClipModule.hpp"


FixedHardClipModule::FixedHardClipModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters)
    : FixedHardClipModule(name,
            ModuleParameter::getFloatOrDefault(parameters, FIXED_HARD_CLIP_MODULE_PARAMETER_THRESHOLD, 1.0f)) {
}

FixedHardClipModule::FixedHardClipModule(std::string name, float threshold): Module(name, 0), threshold(threshold) {
    init();
}

FixedHardClipModule::FixedHardClipModule(const FixedHardClipModule& other)
    : Module(other.name, other.sampleRate) {
    init();
    threshold = other.threshold;
    copyIOs(other);
}

[[nodiscard]] std::unique_ptr<Module> FixedHardClipModule::clone() const {
    return std::make_unique<FixedHardClipModule>(*this);
}

void FixedHardClipModule::init() {
    registerInput(input);
    registerOutput(output);
}

void FixedHardClipModule::envelope() {
    output.value = std::max(-threshold, std::min(threshold, static_cast<float>(input.value)));
}
