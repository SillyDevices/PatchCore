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

#ifndef PATCHCORE_LINEAR_MIX_WITH_LIMIT_MODULE_HPP
#define PATCHCORE_LINEAR_MIX_WITH_LIMIT_MODULE_HPP

#include "patchcore/modules/LinearMixModule.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define LIMITED_MIX_PARAMETER_LIMIT "limit"

class LinearMixWithLimitModule : public LinearMixModule {
public:
    LinearMixWithLimitModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    explicit LinearMixWithLimitModule(std::string name, float limit = 1.0f);
    LinearMixWithLimitModule(const LinearMixWithLimitModule& other);
    [[nodiscard]] std::unique_ptr<Module> clone() const override;
    virtual ~LinearMixWithLimitModule() = default;

public:
    void envelope() override;

private:
    float limit;
};


#endif //PATCHCORE_LINEAR_MIX_WITH_LIMIT_MODULE_HPP
