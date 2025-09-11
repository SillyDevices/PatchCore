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


#ifndef PATCHCORE_CPULOADCOUNTER_HPP
#define PATCHCORE_CPULOADCOUNTER_HPP

#include <map>

class CpuLoadCounter {
private:
    struct CpuLoad {
        int count = 0;
        double load = 100;
    };

public:
    CpuLoadCounter();
    virtual ~CpuLoadCounter() = default;
public:
    void reset();
    void updateCpuLoad(int cpuId, double load);
    int getBestCpuId();
private:
    std::map<int, CpuLoad> cpuLoadMap = std::map<int, CpuLoad>();
};


#endif //PATCHCORE_CPULOADCOUNTER_HPP
