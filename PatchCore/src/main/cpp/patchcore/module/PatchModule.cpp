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
#include "patchcore/module/input/user/ProxyModuleBoolUserInput.hpp"
#include "patchcore/module/input/user/ProxyModuleEnumUserInput.hpp"
#include "patchcore/module/input/user/ProxyModuleFloatUserInput.hpp"
#include "patchcore/module/PolyModule.hpp"
#include <stdexcept>


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
    for (auto &input: other._proxyModuleInputs) {
        cloneProxyInput(input);
    }
    for (auto &output: other._proxyModuleOutputs){
        cloneProxyOutput(output);
    }
    for (auto &input: other._proxyModuleUserInputs) {
        cloneProxyUserInput(input);
    }
    //clone router patches and _modulesToEnvelope and _interpolatedInputsToEnvelope
    clonePatches(other._router);
}

std::unique_ptr<Module> PatchModule::clone() const {
    return std::make_unique<PatchModule>(*this);
}

//TODO add destructor to clear vectors with pointers
PatchModule::~PatchModule() {
    //clear proxy inputs and outputs
    for (auto &input: _proxyModuleInputs) {
        delete input;
    }
    _proxyModuleInputs.clear();
    for (auto &output: _proxyModuleOutputs) {
        delete output;
    }
    _proxyModuleOutputs.clear();
    for (auto &userInput: _proxyModuleUserInputs) {
        delete userInput;
    }
    _proxyModuleUserInputs.clear();
};

void PatchModule::onStartBuffer(int size) {
    for (const auto &module: _modulesToAlwaysEnvelope) {
        module->onStartBuffer(size);
    }
    for (const auto &module: _modulesToEnvelope) {
        module->onStartBuffer(size);
    }
}

void PatchModule::envelope() {
    //need to sync router.envelope
    //it's not a best option to do that
    std::lock_guard<std::mutex> lock(routerMutex);
    for (const auto input: _proxyModuleInputs) {
        input->envelope();
    }
    envelopeModules();
    _router.envelope();
    for (auto output: _proxyModuleOutputs) {
        output->envelope();
    }
}

void PatchModule::envelopeModules() {

    for (auto &input: _interpolatedInputsToAlwaysEnvelope) {
        input->envelope();
    }
    //TODO move this to the module itself
    for (auto &input: _interpolatedInputsToEnvelope) {
        input->envelope();
    }
    for (auto &module: _modulesToAlwaysEnvelope) {
        module->envelope();
    }
    for (auto &module: _modulesToEnvelope) {
        module->envelope();
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

    auto inputs = added->getInterpolatedInputs();
    for (auto input: inputs) {
        _inputs.push_back(input);
    }

    //TODO rename method to needAlwaysEnvelope or isMetaModule or something like that
    if (added->needEnvelopeOnInputConnection()) {
        addModuleToEnvelope(added);
    }

    return added;
}

void PatchModule::clearModules() {
    clearModulesToEnvelope();
    _modules.clear();
    _router.clearModules();
    _inputs.clear();
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
        _modulesToEnvelope.clear();
        _interpolatedInputsToEnvelope.clear();

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
        auto fromModule = from->getModule();
        auto toModule = to->getModule();
        _router.add(from, to);
        //TODO compute a graph and envelope only modules that are in the graph
        if (fromModule != nullptr) { //add source module to envelope list if it not in it already
            if (std::count(_modulesToAlwaysEnvelope.begin(), _modulesToAlwaysEnvelope.end(), fromModule) <= 0) {
                if (std::count(_modulesToEnvelope.begin(), _modulesToEnvelope.end(), fromModule) <= 0) {
                    _modulesToEnvelope.push_back(fromModule);
                    auto inputs = fromModule->getInterpolatedInputs();
                    for (auto input: inputs) {
                        if (std::count(_interpolatedInputsToEnvelope.begin(), _interpolatedInputsToEnvelope.end(), input) <= 0) {
                            if (std::count(_interpolatedInputsToAlwaysEnvelope.begin(), _interpolatedInputsToAlwaysEnvelope.end(), input) <= 0) {
                                _interpolatedInputsToEnvelope.push_back(input);
                            }
                        }
                    }
                }
            }
        }
        if (toModule != nullptr) { //add destination module to envelope list if it not in it already
            if (toModule->needEnvelopeOnInputConnection()) {
                if (std::count(_modulesToEnvelope.begin(), _modulesToEnvelope.end(), toModule) <= 0) {
                    _modulesToEnvelope.push_back(toModule);
                    auto inputs = toModule->getInterpolatedInputs();
                    for (auto input: inputs) {
                        if (std::count(_interpolatedInputsToEnvelope.begin(), _interpolatedInputsToEnvelope.end(), input) <= 0) {
                            if (std::count(_interpolatedInputsToAlwaysEnvelope.begin(), _interpolatedInputsToAlwaysEnvelope.end(), input) <= 0) {
                                _interpolatedInputsToEnvelope.push_back(input);
                            }
                        }
                    }
                }
            }
        }
    }
}

void PatchModule::clonePatches(const Router &router) {
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


ProxyModuleInput* PatchModule::addInput(ModuleInput* input, const std::string& withName) {
    if (input == nullptr) throw std::invalid_argument("Input cannot be null");

    std::lock_guard<std::mutex> lock(_mutex);

    addModuleToEnvelope(input->getModule());
    ProxyModuleInput* proxyInput = input->createProxy(withName);
    _proxyModuleInputs.push_back(proxyInput);

    registerInput(*proxyInput, withName);
    return proxyInput;
}

ProxyModuleOutput* PatchModule::addOutput(ModuleOutput* output, const std::string& withName) {
    if (output == nullptr) throw std::invalid_argument("Output cannot be null");

    std::lock_guard<std::mutex> lock(_mutex);

    addModuleToEnvelope(output->getModule());
    ProxyModuleOutput* proxyOutput = output->createProxy(withName);
    _proxyModuleOutputs.push_back(proxyOutput);

    registerOutput(*proxyOutput, withName);
    return proxyOutput;
}

UserInput* PatchModule::addUserInput(UserInput* input, const std::string& withName) {
    if (input == nullptr) throw std::invalid_argument("UserInput cannot be null");
    ProxyModuleUserInput *proxyUserInput = input->createProxy(withName);
    _proxyModuleUserInputs.push_back(proxyUserInput);
    auto userInput = dynamic_cast<UserInput *>(proxyUserInput);
    if (userInput == nullptr) throw std::runtime_error("PolyModule::addUserInput: casted input is not a UserInput");
    registerUserInput(*userInput, withName);
    return userInput;
}

void PatchModule::cloneProxyInput(const ProxyModuleInput* input) {
    auto withName = input->getName();
    auto realInput = input->getModuleInput();
    auto newInput = findInputByClone(*realInput);
    if (newInput == nullptr) {
        throw std::runtime_error("Input " + realInput->getName() + " not found in module " + realInput->getModule()->getModuleName());
    }
    addInput(newInput, withName);
}

void PatchModule::cloneProxyOutput(const ProxyModuleOutput* output) {
    auto withName = output->getName();
    auto realOutput = output->getModuleOutput();
    auto newOutput = findOutputByClone(*realOutput);
    if (newOutput == nullptr) {
        throw std::runtime_error("Output " + realOutput->getName() + " not found in module " + realOutput->getModule()->getModuleName());
    }
    addOutput(newOutput, withName);
}

void PatchModule::cloneProxyUserInput(const ProxyModuleUserInput* input) {
    //TODO
    auto casted = dynamic_cast<const ProxyModuleUserInput*>(input);
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
    addUserInput(newInput, name);
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


void PatchModule::addModuleToEnvelope(Module *module) {
    std::lock_guard<std::mutex> lock(routerMutex);
    if (std::count(_modulesToAlwaysEnvelope.begin(), _modulesToAlwaysEnvelope.end(), module) == 0) {
        _modulesToAlwaysEnvelope.push_back(module);
    }
    auto inputs = module->getInterpolatedInputs();
    for (auto input: inputs) {
        if (std::count(_interpolatedInputsToAlwaysEnvelope.begin(), _interpolatedInputsToAlwaysEnvelope.end(), input) <= 0) {
            _interpolatedInputsToAlwaysEnvelope.push_back(input);
        }
    }
}

void PatchModule::clearModulesToEnvelope() {
    std::lock_guard<std::mutex> lock(routerMutex);
    _modulesToAlwaysEnvelope.clear();
    _interpolatedInputsToAlwaysEnvelope.clear();
}

std::unique_ptr<PolyProxyModule> PatchModule::createPolyModuleProxy(PolyModule* polyModule) const {
    return Module::createPolyModuleProxy(polyModule);
}