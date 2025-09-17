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


#include "PatchModuleWrapper.h"
#include "ModuleParameterWrapper.h"

#include <patchCore/module/factory/ModuleFactory.hpp>
#include <patchCore/module/PatchModule.hpp>

uintptr_t patchModuleNew(uintptr_t module_factory_pointer, char* name, int sample_rate) {
    auto *factory = reinterpret_cast<ModuleFactory *>(module_factory_pointer);
    if (factory == nullptr) throw std::runtime_error("ModuleFactory pointer is null");
    std::string nameString(name);
    PatchModule *patchModule = new PatchModule(factory, nameString, sample_rate);
    return reinterpret_cast<uintptr_t>(static_cast<Module*>(patchModule));
}

void patchModuleDelete(uintptr_t patch_module_pointer) {
    auto modulePtr = reinterpret_cast<Module *>(patch_module_pointer);
    if (modulePtr == nullptr) throw std::runtime_error("PatchModule pointer is null");
    delete modulePtr;
}

uintptr_t patchModuleCreateModule(uintptr_t patch_module_pointer, char* module_type, char* module_name, ModuleParameterWrapper* parameters, unsigned long parameters_count) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    std::string moduleTypeNameString(module_type);
    std::string moduleNameString(module_name);
    
    std::map<std::string, ModuleParameter> nativeParameters;

    for (unsigned long i = 0; i < parameters_count; ++i) {
        ModuleParameterWrapper &paramWrapper = parameters[i];
        std::string key = std::string(paramWrapper.key);
        switch (paramWrapper.type) {
            case 0: // enum
            {
                ModuleParameter paramEnum = ModuleParameter(paramWrapper.enumValue);
                nativeParameters.insert({ key, paramEnum});
            }
                break;
            case 1: // float
            {
                ModuleParameter paramFloat = ModuleParameter(paramWrapper.floatValue);
                nativeParameters.insert({key, paramFloat});
            }
                break;
            case 2: // bool
            {
                ModuleParameter paramBool = ModuleParameter(paramWrapper.boolValue);
                nativeParameters.insert({ key, paramBool});
            }
                break;
            default:
                throw std::runtime_error("Unknown parameter type");
        }
    }

    auto result = patchModule->createModule(moduleTypeNameString, moduleNameString, nativeParameters);

    return reinterpret_cast<uintptr_t>(result);
}

void patchModuleAddModule(uintptr_t patch_module_pointer, uintptr_t managed_module_pointer) {
    //remove pointer to managed_module_pointer from java object after this call, or just do not delete this poiner
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    auto *managedModulePtr = reinterpret_cast<Module *>(managed_module_pointer);
    if (managedModulePtr == nullptr) throw std::runtime_error("ManagedModule pointer is not a ManagedNativePtr<Module>");
    patchModule->addModule(std::unique_ptr<Module>(managedModulePtr));
}

uintptr_t patchModuleGetModule(uintptr_t patch_module_pointer, char* module_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    std::string moduleNameString(module_name);
    Module *resultModule = patchModule->getModule(moduleNameString);
    return reinterpret_cast<uintptr_t>(resultModule);
}

void patchModuleAddInput(uintptr_t patch_module_pointer, uintptr_t input_pointer, char* input_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    
    auto input = reinterpret_cast<ModuleInput *>(input_pointer);
    if (input == nullptr) throw std::runtime_error("Input pointer is null");
    std::string inputNameString(input_name);
    patchModule->addInput(input, inputNameString);
}

void patchModuleAddOutput(uintptr_t patch_module_pointer, uintptr_t output_pointer, char* output_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    
    auto output = reinterpret_cast<ModuleOutput *>(output_pointer);
    if (output == nullptr) throw std::runtime_error("Output pointer is null");
    std::string outputNameString(output_name);
    patchModule->addOutput(output, outputNameString);
}

void patchModuleAddUserInput(uintptr_t patch_module_pointer, uintptr_t user_input_pointer, char* input_name) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto *patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");

    auto userInput = reinterpret_cast<UserInput *>(user_input_pointer);
    if (userInput == nullptr) throw std::runtime_error("UserInput pointer is null");
    std::string inputNameString(input_name);
    patchModule->addUserInput(userInput, inputNameString);
}

void patchModuleAddPatch(uintptr_t patch_module_pointer, uintptr_t from_output_pointer, uintptr_t to_input_pointer) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    auto fromOutput = reinterpret_cast<ModuleOutput *>(from_output_pointer);
    if (fromOutput == nullptr) throw std::runtime_error("From output pointer is null");
    auto toInput = reinterpret_cast<ModuleInput *>(to_input_pointer);
    if (toInput == nullptr) throw std::runtime_error("To input pointer is null");
    patchModule->addPatch(fromOutput, toInput);
}

void patchModuleResetPatch(uintptr_t patch_module_pointer) {
    auto module = reinterpret_cast<Module *>(patch_module_pointer);
    auto patchModule = dynamic_cast<PatchModule *>(module);
    if (patchModule == nullptr) throw std::runtime_error("PatchModule pointer is null");
    patchModule->resetPatch();
}
