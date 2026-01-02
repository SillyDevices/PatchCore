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
#include "patchcore/module/output/ProxyModuleOutput.hpp"
#include "patchcore/module/input/ProxyModuleInput.hpp"
#include "patchcore/module/input/ProxyModuleUserInput.hpp"

#include <string>
#include <vector>

/*
 * PatchModule can be used to create subModule with multiple modules inside, connections between them and inputs/outputs
 * outward facing inputs/outputs should be exposed manually by calling addInput/addOutput
 * userInputs are available througth getModule("module_name")->getUserInput("input_name") or
 * exposed by addUserInput
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
    virtual ProxyModuleInput* addInput(ModuleInput* input, const std::string& withName);
    virtual ProxyModuleOutput* addOutput(ModuleOutput* output, const std::string& withName);
    virtual UserInput* addUserInput(UserInput* input, const std::string& withName);
private:
    void cloneProxyInput(const ProxyModuleInput* input);
    void cloneProxyOutput(const ProxyModuleOutput* output);
    void cloneProxyUserInput(const ProxyModuleUserInput* input);
private:
    ModuleInput* findInputByClone(const ModuleInput &input) const;
    ModuleOutput* findOutputByClone(const ModuleOutput &output) const;

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

    std::vector<ProxyModuleOutput *> _proxyModuleOutputs;
    std::vector<ProxyModuleInput *> _proxyModuleInputs;
    std::vector<ProxyModuleUserInput *> _proxyModuleUserInputs;

};

#endif //PATCHCORE_PATCHMODULE_HPP
