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

#ifndef PATCHCORE_PATCHMODULE_HPP
#define PATCHCORE_PATCHMODULE_HPP

#include "patchcore/module/factory/ModuleFactory.hpp"
#include "patchcore/module/Module.hpp"
#include "patchcore/module/router/Router.hpp"
#include "patchcore/module/router/GraphRouter.hpp"
#include "patchcore/module/output/ExposedModuleOutput.hpp"
#include "patchcore/module/input/ExposedModuleInput.hpp"
#include "patchcore/module/input/ExposedModuleUserInput.hpp"

#include <string>
#include <vector>

/*
 * PatchModule can be used to create subModule with multiple modules inside, connections between them and inputs/outputs
 * outward facing inputs/outputs should be exposed manually by calling exposeInput/exposeOutput
 * userInputs are available througth getModule("module_name")->getUserInput("input_name") or
 * exposed by exposeUserInput
 * the main usecase is to create a module dynamically
 */

class PatchModule : public Module {
public:
    PatchModule(ModuleFactory *factory, std::string name, int sampleRate);
    PatchModule(const PatchModule &other); // copy constructor for clone
public:
    std::unique_ptr<Module> clone() const override;
    virtual ~PatchModule() override;
protected:
    PatchModule();
    //Module interface
public:
    void onStartBuffer(int size) override;
    void envelope() override;
    //PatchModule specific
    //creates a new input/output from existing ModuleInput/ModuleOutput
public:
    //TODO rename to createProxyInput/Output/UserInput
    virtual ExposedModuleInput* exposeInput(ModuleInput* input, const std::string& withName);
    virtual ExposedModuleOutput* exposeOutput(ModuleOutput* output, const std::string& withName);
    virtual UserInput* exposeUserInput(UserInput* input, const std::string& withName);
private:
    void cloneExposedInput(const ExposedModuleInput* input);
    void cloneExposedOutput(const ExposedModuleOutput* output);
    void cloneExposedUserInput(const ExposedModuleUserInput* input);
private:
    ModuleInput* findInputByClone(const ModuleInput &input) const;
    ModuleOutput* findOutputByClone(const ModuleOutput &output) const;
    Module* findDirectOwnedModule(const Module* module) const;
    bool containsModuleRecursive(const PatchModule& patch, const Module* target) const;

    //PatchModule specific
public:
    //creates a new module with factory call
    virtual Module* createModule(const std::string& moduleTypeName, const std::string& moduleName, const std::map<std::string, ModuleParameter>& parameters);
    //move an external module into the PatchModule
    virtual Module* addModule(std::unique_ptr<Module> module);
    //remove all modules from the PatchModule
    //TODO add method to remove single module
    virtual void clearModules();
    //get a module by name. Note must be only legit way to get a module
    virtual Module* getModule(const std::string& moduleName) const;


    //PatchModule Router specific
public:
    virtual void resetPatch();
    virtual void addPatch(ModuleOutput* output, ModuleInput* input);
    virtual void removePatch(ModuleOutput* output, ModuleInput* input);
private:
    void clonePatches(const AbstractRouter &router);

    //Module copy and poly module creation
public:
    virtual std::unique_ptr<PolyProxyModule> createPolyModuleProxy(PolyModule* polyModule) const override;

protected:
    std::mutex _mutex;
private:
    ModuleFactory* _factory;

    std::mutex routerMutex;
//    Router _router = Router(this);
    GraphRouter _router = GraphRouter(this);

    std::vector<std::unique_ptr<Module>> _modules  = std::vector<std::unique_ptr<Module>>();

    std::vector<ExposedModuleOutput *> _exposedOutputs;
    std::vector<ExposedModuleInput *> _exposedInputs;
    std::vector<ExposedModuleUserInput *> _exposedUserInputs;

};

#endif //PATCHCORE_PATCHMODULE_HPP
