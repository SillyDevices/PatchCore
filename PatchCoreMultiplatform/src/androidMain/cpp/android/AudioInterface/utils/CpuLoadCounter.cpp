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


#include "CpuLoadCounter.hpp"

CpuLoadCounter::CpuLoadCounter() {

}

void CpuLoadCounter::reset() {
    cpuLoadMap.clear();
}

void CpuLoadCounter::updateCpuLoad(int cpuId, double load) {
    if (cpuId == -1) return;

//    auto lastLoad = proxy.get()->getCurrentLoad();
//    if (lastCpuId != -1){
//        auto count = cpuLoadMap[lastCpuId].count + 1;
//        auto load = cpuLoadMap[lastCpuId].load;
//        load = load*0.90 + lastLoad*0.1;
//        cpuLoadMap[lastCpuId] = { count, load };
//    }

    auto it = cpuLoadMap.find(cpuId);
    if (it != cpuLoadMap.end()){
        auto count = it->second.count + 1;
        auto lastLoad = it->second.load;
        lastLoad = lastLoad*0.90 + load*0.1;
        it->second = { count, lastLoad };
    } else {
        cpuLoadMap[cpuId] = { 1, load };
    }
}

int CpuLoadCounter::getBestCpuId() {

    auto count = cpuLoadMap.size();
    auto cycles = 0;
    for (auto &item :cpuLoadMap) {
        cycles += item.second.count;
    }
    auto cycleLimit = (cycles / count) / 4;
    auto minLoad = 100.0;
    auto bestCpu = -1;
    for (auto &item :cpuLoadMap) {
        if (item.second.load < minLoad && item.second.count > cycleLimit){
            minLoad = item.second.load;
            bestCpu = item.first;
        }
    }
    return bestCpu;
}


