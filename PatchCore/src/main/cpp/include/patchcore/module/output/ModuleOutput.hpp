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

#ifndef PATCHCORE_MODULEOUTPUT_HPP
#define PATCHCORE_MODULEOUTPUT_HPP

#include "patchcore/module/output/Output.hpp"
#include <string>

class Module;
class ExposedModuleOutput;

class ModuleOutput: Output {

public:

    explicit ModuleOutput(std::string outputName);
    virtual ~ModuleOutput() = default;

    [[nodiscard]]
    inline std::string getName() const {
        return name;
    };

public:

    void setModule(Module* module);

    //TODO do i really need to inline this? it not called from audio thread anyway
    [[nodiscard]]
    inline Module* getModule() const {
        return _module;
    }

    virtual ExposedModuleOutput* createExposed(const std::string& withName);
    void onStartBuffer(int size) {
        value.onStartBuffer(size);
    }

    void advanceSample() {
        value.advanceSample();
    }

    const std::vector<float>& getBuffer() const {
        return value.getBuffer();
    }

    [[nodiscard]]
    bool hasProxyOutput() const;


private:
    std::string name;
    Module* _module = nullptr;

    bool hasProxy = false;

public:
    BufferedValue value = BufferedValue(0.0f);

};

#endif //PATCHCORE_MODULEOUTPUT_HPP
