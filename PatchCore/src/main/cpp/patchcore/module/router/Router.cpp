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

#include "patchcore/module/router/Router.hpp"
#include <unordered_set>


Router::Router(Module *parentModule): AbstractRouter(), parentModule(parentModule) {

}

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
        if (output.second->hasProxyOutput()) {
            outputModules.insert(module);
        }
    }
    modules.push_back(module);

    if (module->needEnvelopeOnInputConnection()) {
        outputModules.insert(module);
    }
}

void Router::removeModule(Module *module) {
    throw std::runtime_error("Router::removeModule not implemented yet");
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
    modulesToEnvelope.clear();
    userInputsToEnvelope.clear();
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

void Router::moduleInputChanged(Module *module) {
    //do nothing for now, should work as is.
}

void Router::makeInputsAndOutputs() {

    matrix_inputs.clear();
    matrix_outputs.clear();

    std::unordered_set<Module*> _modulesToEnvelopeSet;

    for (const auto &matrixKV: matrix){
        if (matrixKV.second.empty())
            continue;
        matrixKV.first->value.fill(0.0f);
        matrix_inputs.push_back(matrixKV.first);
        matrix_outputs.push_back(matrixKV.second);

        // collect modules to envelope
        auto module = matrixKV.first->getModule();
        _modulesToEnvelopeSet.insert(module);
        for (const auto &output: matrixKV.second){
            auto outputModule = output->getModule();
            _modulesToEnvelopeSet.insert(outputModule);
        }
    }

    _modulesToEnvelopeSet.insert(outputModules.begin(), outputModules.end());
    //remove parentModule from modules to envelope
    _modulesToEnvelopeSet.erase(parentModule);

    // update modulesToEnvelope vector
    modulesToEnvelope = std::vector<Module*>(_modulesToEnvelopeSet.begin(), _modulesToEnvelopeSet.end());

    //update userInputsToEnvelope
    userInputsToEnvelope.clear();
    std::unordered_set<FloatUserInput*> userInputSet;
    for (const auto &module: modulesToEnvelope){
        auto userInputs = module->getInterpolatedInputs();
        for (auto userInput: userInputs){
            userInputSet.insert(userInput);
        }
    }
    userInputsToEnvelope = std::vector<FloatUserInput*>(userInputSet.begin(), userInputSet.end());

}

std::vector<std::pair<ModuleOutput *, ModuleInput *>> Router::getPatches() const {
    std::vector<std::pair<ModuleOutput *, ModuleInput *>> patches;
    for (const auto &matrixKV: matrix) {
        auto input = matrixKV.first;
        for (const auto &output: matrixKV.second) {
            patches.emplace_back(output, input);
        }
    }
    return patches;
}

void Router::onStartBuffer(int size) {
    BlockContext context;
    context.blockSize = size;
    context.sampleRate = parentModule->getSampleRate();
    context.blockStartSample = 0;
    context.blockStartTimeUs = 0.0;
    onStartBlock(context);
}

void Router::onStartBlock(const BlockContext& context) {
    for (const auto &module: modulesToEnvelope){
        module->onStartBlock(context);
    }
    //TODO move userinput envelope to Module::onStartBlock
    for (const auto &userInput: userInputsToEnvelope){
        userInput->onStartBuffer(context.blockSize);
    }
}

void Router::envelope() {
    throw std::runtime_error("Router::envelope() is deprecated");
}






