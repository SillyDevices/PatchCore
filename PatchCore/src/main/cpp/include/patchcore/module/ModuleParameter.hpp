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

#ifndef PATCHCORE_MODULEPARAMETER_HPP
#define PATCHCORE_MODULEPARAMETER_HPP

#include <map>
#include <string>


class ModuleParameter {
    public:
    enum Type {
        INT,
        FLOAT,
        BOOL
    };
public:
    explicit ModuleParameter(int value);
    explicit ModuleParameter(float value);
    explicit ModuleParameter(bool value);
public:
    ~ModuleParameter() = default;
public:
    Type getType();
    float getFloatValue() const;
    int getIntValue() const ;
    bool getBoolValue() const;
private:
    Type type;

    int intValue;
    float floatValue;
    bool boolValue;
public:
    static bool getBoolOrDefault(const std::map<std::string, ModuleParameter> &parameters, std::string key,
                          bool defaultValue) {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second.getBoolValue();
        }
        return defaultValue;
    }

    static int getIntOrDefault(const std::map<std::string, ModuleParameter> &parameters, std::string key,
                         int defaultValue) {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second.getIntValue();
        }
        return defaultValue;
    }

    static float getFloatOrDefault(const std::map<std::string, ModuleParameter> &parameters, std::string key,
                           float defaultValue) {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second.getFloatValue();
        }
        return defaultValue;
    }
};

#endif //PATCHCORE_MODULEPARAMETER_HPP
