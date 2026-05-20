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

#include "patchcore/module/PatchModule.hpp"
#include "patchcore/module/input/user/ExposedModuleBoolUserInput.hpp"
#include "patchcore/module/input/user/ExposedModuleEnumUserInput.hpp"
#include "patchcore/module/input/user/ExposedModuleFloatUserInput.hpp"
#include "patchcore/module/PolyModule.hpp"
#include "patchcore/module/PolyProxyPatchModule.hpp"
#include <algorithm>
#include <stdexcept>

#ifdef ANDROID
#include <android/log_macros.h>
#undef LOG_TAG
#define LOG_TAG "PatchModule"
#endif


PatchModule::PatchModule(ModuleFactory *factory, std::string name, int sampleRate): Module(name, sampleRate), _factory(factory){

}

PatchModule::PatchModule(const PatchModule &other): Module(other.name, other.sampleRate), _factory(other._factory) {
    //clone modules, includes intialization of
    //_modules and _inputs
    //_modulestoAlwaysEnvelope and _interpolatedInputsToAlwaysEnvelope
    for (auto &module: other._modules) {
        addModule(module->clone());
    }
    //clone _compositeModuleInputs and _compositeModuleOutputs
    for (auto &input: other._exposedInputs) {
        cloneExposedInput(input);
    }
    for (auto &output: other._exposedOutputs){
        cloneExposedOutput(output);
    }
    for (auto &input: other._exposedUserInputs) {
        cloneExposedUserInput(input);
    }
    //clone router patches and _modulesToEnvelope and _interpolatedInputsToEnvelope
    clonePatches(other._router);
}

std::unique_ptr<Module> PatchModule::clone() const {
    return std::make_unique<PatchModule>(*this);
}

//empty constructor for PolyProxyPatchModule
PatchModule::PatchModule(): Module("", 44100) { }

//TODO add destructor to clear vectors with pointers
PatchModule::~PatchModule() {
    //clear proxy inputs and outputs
    for (auto &input: _exposedInputs) {
        delete input;
    }
    _exposedInputs.clear();
    for (auto &output: _exposedOutputs) {
        delete output;
    }
    _exposedOutputs.clear();
    for (auto &userInput: _exposedUserInputs) {
        delete userInput;
    }
    _exposedUserInputs.clear();
};

void PatchModule::onStartBuffer(int size) {
    BlockContext context;
    context.blockSize = size;
    context.sampleRate = sampleRate;
    context.blockStartSample = 0;
    context.blockStartTimeUs = 0.0;
    onStartBlock(context);
}

void PatchModule::onStartBlock(const BlockContext& context) {
    Module::onStartBlock(context);
    std::lock_guard<std::mutex> lock(routerMutex);
    _router.onStartBlock(context);
}

void PatchModule::envelope() {
    throw std::runtime_error("PatchModule::envelope() is deprecated, use processSample/processBlock");
    //need to sync router.envelope
    //it's not a best option to do that
    std::lock_guard<std::mutex> lock(routerMutex);
    processCurrentSample();
}

void PatchModule::processSample(int sampleIndex) {
    (void) sampleIndex;
    std::lock_guard<std::mutex> lock(routerMutex);
    processCurrentSample();
}

void PatchModule::processBlock() {
    std::lock_guard<std::mutex> lock(routerMutex);

    for (const auto input: _exposedInputs) {
        auto* sourceBuffer = input->value.data();
        auto* targetBuffer = input->getModuleInput()->value.data();
        std::copy(sourceBuffer, sourceBuffer + PATCHCORE_BLOCK_SIZE, targetBuffer);
    }

    _router.processBlock();

    for (auto output: _exposedOutputs) {
        auto* sourceBuffer = output->getModuleOutput()->value.data();
        auto* targetBuffer = output->value.data();
        std::copy(sourceBuffer, sourceBuffer + PATCHCORE_BLOCK_SIZE, targetBuffer);
    }
}

void PatchModule::processCurrentSample() {
    // What about _exposedUserInputs? who envelope them? maybe parrent Module does it?

    for (const auto input: _exposedInputs) {
        input->envelope();
    }

    _router.envelope();

    for (auto output: _exposedOutputs) {
        output->envelope();
    }
}



Module* PatchModule::createModule(const std::string &moduleTypeName, const std::string &moduleName,
                                       const std::map<std::string, ModuleParameter> &parameters) {
    auto module = _factory->createModule(moduleTypeName, moduleName, sampleRate, parameters);
    if (module == nullptr) {
        throw std::runtime_error("Module creation failed for type: " + moduleTypeName);
    }
    return addModule(std::move(module));

}


Module* PatchModule::addModule(std::unique_ptr<Module> module) {
    _modules.push_back(std::move(module));
    auto added = _modules.back().get();
    _router.addModule(added);
    return added;
}

void PatchModule::clearModules() {
    _modules.clear();
    _router.clearModules();
    //TODO clear proxy inputs and outputs?
}

Module* PatchModule::getModule(const std::string &moduleName) const {
    for (auto& module : _modules) {
        if (module->getModuleName() == moduleName) {
            return module.get();
        }
    }
    return nullptr;
}




void PatchModule::resetPatch() {
    {
        std::lock_guard<std::mutex> lock(routerMutex);
        _router.reset();

        //TODO not used!!! check and delete
        for (const auto &module: _modules){
            auto inputs = module->getModuleInputs();
            for (const auto &kv: inputs) {
                kv.second->disconnect();
            }
        }
    }
}

void PatchModule::addPatch(ModuleOutput* from, ModuleInput* to) {
    std::lock_guard<std::mutex> lock(routerMutex);
    if (from != nullptr && to != nullptr) {
        _router.add(from, to);
    }
}

void PatchModule::removePatch(ModuleOutput* from, ModuleInput* to) {
    std::lock_guard<std::mutex> lock(routerMutex);
    if (from != nullptr && to != nullptr) {
        _router.remove(from, to);
    }
}

void PatchModule::clonePatches(const AbstractRouter &router) {
    for (const auto &patch: router.getPatches()) {
        auto originalFrom = patch.first;
        auto originalTo = patch.second;
        if (originalFrom != nullptr && originalTo != nullptr) {
            auto from = findOutputByClone(*originalFrom);
            auto to = findInputByClone(*originalTo);
            if (from == nullptr || to == nullptr) {
                throw std::runtime_error("PatchModule::clonePatches: Could not find original input or output for patch");
            }
            addPatch(from, to);
        }
    }
}


ExposedModuleInput* PatchModule::exposeInput(ModuleInput* input, const std::string& withName) {
    if (input == nullptr) throw std::invalid_argument("Input cannot be null");

    std::lock_guard<std::mutex> lock(_mutex);

    ExposedModuleInput* exposedInput = input->createExposed(withName);
    _exposedInputs.push_back(exposedInput);
    //TODO check if router needs update

    registerInput(*exposedInput, withName);
    return exposedInput;
}

ExposedModuleOutput* PatchModule::exposeOutput(ModuleOutput* output, const std::string& withName) {
    if (output == nullptr) throw std::invalid_argument("Output cannot be null");

    std::lock_guard<std::mutex> lock(_mutex);

    Module* outputOwner = output->getModule();
    Module* directOwnedModule = outputOwner != nullptr ? findDirectOwnedModule(outputOwner) : nullptr;
    ExposedModuleOutput* exposedOutput = output->createExposed(withName);
    if (directOwnedModule != nullptr && directOwnedModule != outputOwner) {
        directOwnedModule->onProxyOutputCreated(output, withName);
    }
    _exposedOutputs.push_back(exposedOutput);
    registerOutput(*exposedOutput, withName);
    _router.moduleInputChanged(directOwnedModule != nullptr ? directOwnedModule : outputOwner);
    return exposedOutput;
}

UserInput* PatchModule::exposeUserInput(UserInput* input, const std::string& withName) {
    if (input == nullptr) throw std::invalid_argument("UserInput cannot be null");
    ExposedModuleUserInput *exposedUserInput = input->createExposed(withName);
    _exposedUserInputs.push_back(exposedUserInput);
    auto userInput = dynamic_cast<UserInput *>(exposedUserInput);
    if (userInput == nullptr) throw std::runtime_error("PolyModule::exposeUserInput: casted input is not a UserInput");
    registerUserInput(*userInput, withName);
    return userInput;
}

void PatchModule::cloneExposedInput(const ExposedModuleInput* input) {
    auto withName = input->getName();
    auto realInput = input->getModuleInput();
    auto newInput = findInputByClone(*realInput);
    if (newInput == nullptr) {
        throw std::runtime_error("Input " + realInput->getName() + " not found in module " + realInput->getModule()->getModuleName());
    }
    exposeInput(newInput, withName);
}

void PatchModule::cloneExposedOutput(const ExposedModuleOutput* output) {
    auto withName = output->getName();
    auto realOutput = output->getModuleOutput();
    auto newOutput = findOutputByClone(*realOutput);
    if (newOutput == nullptr) {
        throw std::runtime_error("Output " + realOutput->getName() + " not found in module " + realOutput->getModule()->getModuleName());
    }
    exposeOutput(newOutput, withName);
}

void PatchModule::cloneExposedUserInput(const ExposedModuleUserInput* input) {
    //TODO
    auto casted = dynamic_cast<const ExposedModuleUserInput*>(input);
    auto realInput = casted->getUserInput();
    auto realInputModuleName = realInput->getModule()->getModuleName();
    auto realInputName = realInput->getName();
    auto castedInput = dynamic_cast<const UserInput *>(input);
    auto name = castedInput->getName();
    auto newModule = getModule(realInputModuleName);
    if (newModule == nullptr) {
        throw std::runtime_error("Module " + realInputModuleName + " not found in patch");
    }
    auto newInput = newModule->getUserInput(realInputName);
    if (newInput == nullptr) {
        throw std::runtime_error("User Input " + realInputName + " not found in module " + realInputModuleName);
    }
    exposeUserInput(newInput, name);
}

ModuleInput* PatchModule::findInputByClone(const ModuleInput &input) const {
    auto moduleName = input.getModule()->getModuleName();
    auto inputName = input.getName();
    auto module = getModule(moduleName);
    if (module != nullptr) {
        auto found = module->getModuleInput(inputName);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

ModuleOutput *PatchModule::findOutputByClone(const ModuleOutput &output) const {
    auto moduleName = output.getModule()->getModuleName();
    auto outputName = output.getName();
    auto module = getModule(moduleName);
    if (module != nullptr) {
        auto found = module->getModuleOutput(outputName);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

bool PatchModule::containsModuleRecursive(const PatchModule& patch, const Module* target) const {
    for (const auto& module : patch._modules) {
        if (module.get() == target) {
            return true;
        }
        auto childPatch = dynamic_cast<PatchModule*>(module.get());
        if (childPatch != nullptr && containsModuleRecursive(*childPatch, target)) {
            return true;
        }
    }
    return false;
}

Module* PatchModule::findDirectOwnedModule(const Module* module) const {
    for (const auto& ownedModule : _modules) {
        if (ownedModule.get() == module) {
            return ownedModule.get();
        }
        auto childPatch = dynamic_cast<PatchModule*>(ownedModule.get());
        if (childPatch != nullptr && containsModuleRecursive(*childPatch, module)) {
            return ownedModule.get();
        }
    }
    return nullptr;
}

std::unique_ptr<PolyProxyModule> PatchModule::createPolyModuleProxy(PolyModule* polyModule) const {
    //TODO create special proxy with add delete patch functions
    return std::make_unique<PolyProxyPatchModule>(_factory, this, polyModule);
}
