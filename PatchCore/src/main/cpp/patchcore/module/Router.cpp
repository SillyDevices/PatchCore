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

#include "patchcore/module/Router.hpp"


void Router::clearModules() {
    inputs.clear();
    outputs.clear();
    modules.clear();
    reset();
}

void Router::addModule(Module *module) {
    auto inputsToAdd = module->getModuleInputs();
    auto outputsToAdd = module->getModuleOutputs();
    for (const auto &input: inputsToAdd) {
        inputs[module->getModuleName() + "_" + input.first] = input.second;
    }
    for (const auto &output: outputsToAdd) {
        outputs[module->getModuleName() + "_" + output.first] = output.second;
    }
    modules.push_back(module);
}

void Router::reset() {
    matrix.clear();
    for (const auto &input: matrix_inputs) {
        input->disconnect();
    }
    matrix_inputs.clear();
    matrix_outputs.clear();
    for (const auto &inputKV: inputs) {
        auto input = inputKV.second;
        matrix[input] = std::vector<ModuleOutput *>();
    }
};

void Router::add(ModuleOutput *from, ModuleInput *to) {
    to->connect(from);
    matrix[to].push_back(from);
    makeInputsAndOutputs();
}

void Router::remove(ModuleOutput *from, ModuleInput *to) {
    auto itMatrix = matrix.find(to);
    if (itMatrix == matrix.end()) {
        return;
    }

    auto &fromVector = itMatrix->second;
    auto it = std::remove(fromVector.begin(), fromVector.end(), from);
    if (it != fromVector.end()) {
        fromVector.erase(it, fromVector.end());
    }

    if (fromVector.empty()) {
        matrix.erase(itMatrix);
        to->disconnect();
    }

    makeInputsAndOutputs();
}

void Router::makeInputsAndOutputs() {
    matrix_inputs.clear();
    matrix_outputs.clear();
    for (const auto &matrixKV: matrix){
        if (matrixKV.second.empty())
            continue;
        matrixKV.first->value = 0.0f;
        matrix_inputs.push_back(matrixKV.first);
        matrix_outputs.push_back(matrixKV.second);
    }
}

const std::vector<std::pair<ModuleOutput *, ModuleInput *>> Router::getPatches() const {
    std::vector<std::pair<ModuleOutput *, ModuleInput *>> patches;
    for (const auto &matrixKV: matrix) {
        auto input = matrixKV.first;
        for (const auto &output: matrixKV.second) {
            patches.emplace_back(output, input);
        }
    }
    return patches;
}


void Router::envelope() {
    int counter = 0;
    for (const auto &input: matrix_inputs){
        float sum = .0f;
        for (const auto &output: matrix_outputs.at(counter)){
            sum += output->value;
        }
        counter++;
        input->value = sum;
    }
    //TODO envelope userInputs
    //TODO envelope modules
}






