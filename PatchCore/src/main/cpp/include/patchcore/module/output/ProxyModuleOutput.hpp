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

#ifndef PATCHCORE_PROXYMODULEOUTPUT_HPP
#define PATCHCORE_PROXYMODULEOUTPUT_HPP

#include "patchcore/module/output/ModuleOutput.hpp"

class ProxyModuleOutput: public ModuleOutput {

public:
    ProxyModuleOutput(const std::string& outputName, ModuleOutput* output): ModuleOutput(outputName), _output(output) {};
    virtual ~ProxyModuleOutput() = default;

    void envelope(){
        value = _output->value;
    }

    [[nodiscard]]
    ModuleOutput* getModuleOutput() const{
        return _output;
    }

private:
    ModuleOutput* _output;

};

#endif //PATCHCORE_PROXYMODULEOUTPUT_HPP
