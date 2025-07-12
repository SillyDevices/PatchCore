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

#ifndef PATCHCORE_FADE_ENVELOPE_MODULE_HPP
#define PATCHCORE_FADE_ENVELOPE_MODULE_HPP

#include "patchcore/modules/envelope/BaseEnvelope.hpp"
#include "patchcore/module/Module.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/FloatUserInput.hpp"

#define FADE_ENVELOPE_MODULE_PARAMETER_TIME "time"
#define FADE_ENVELOPE_MODULE_PARAMETER_IS_OPENED "isOpened"

#define FADE_ENVELOPE_MODULE_INPUT "gate"
#define FADE_ENVELOPE_MODULE_OUTPUT "out"

class FadeEnvelopeModule : public Module {
public:
    FadeEnvelopeModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    FadeEnvelopeModule(std::string name, int sampleRate, float time = 0.3, bool isOpened = true);
    FadeEnvelopeModule(const FadeEnvelopeModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~FadeEnvelopeModule() = default;

public:
    void envelope() override;

private:
    enum FaderState { opened , closed, opening, closing };
private:
    FloatUserInput input = FloatUserInput(FADE_ENVELOPE_MODULE_INPUT);
    ModuleOutput output = ModuleOutput(FADE_ENVELOPE_MODULE_OUTPUT);

    int fadeSamples;
    float delta;
    FaderState state = opened;
    float outputValue = 1.0f;
};


#endif //PATCHCORE_FADE_ENVELOPE_MODULE_HPP
