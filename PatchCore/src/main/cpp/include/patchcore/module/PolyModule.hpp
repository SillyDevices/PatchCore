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

#ifndef PATCHCORE_POLYMODULE_HPP
#define PATCHCORE_POLYMODULE_HPP

#include "patchcore/module/Module.hpp"
#include "patchcore/module/PatchModule.hpp"
#include "patchcore/module/PolyProxyModule.hpp"
#include <string>


//the main idea behind this class is to provide an PatchModule interface for polyphonic modules
//it's acts like a container for multiple PatchModule instances, each representing a voice and proxies
//methods to all voices
class PolyModule : public PatchModule {
public:
    PolyModule(ModuleFactory *factory, std::string name, int sampleRate, int voiceCount);
    virtual ~PolyModule() = default;

    //Module interface
public:
    void onStartBuffer(int size) override;
    void envelope() override;

    //PatchModule specific
    //creates a new input/output from existing ModuleInput/ModuleOutput
public:
    ProxyModuleInput* addInput(ModuleInput* input, const std::string& withName) override;
    ProxyModuleOutput* addOutput(ModuleOutput* output, const std::string& withName) override;
    //don't know in what case this is useful, but it is here for consistency
    UserInput* addUserInput(UserInput* input, const std::string& withName) override;
    //PatchModule specific
public:
    virtual Module* createModule(const std::string& moduleTypeName, const std::string& moduleName, const std::map<std::string, ModuleParameter>& parameters) override;
    virtual Module* addModule(std::unique_ptr<Module> module) override;
    virtual void clearModules() override;
    virtual Module* getModule(const std::string& moduleName) const override;
    //PatchModule Router specific
public:
    virtual void addPatch(ModuleOutput* output, ModuleInput* input) override;
    virtual void resetPatch() override;


    // polyphony support
    size_t getVoiceCount() const;

    PatchModule* getVoice(size_t index);
//    size_t getActiveVoiceCount() const;
//    void setActiveVoiceCount(size_t count);

protected:
    std::vector<std::unique_ptr<PatchModule>> voices;
    size_t voiceCount;

//    std::unordered_map<std::string, ModuleInput *> inputs;
//    std::unordered_map<std::string, ModuleOutput *> outputs;
//    std::unordered_map<std::string, UserInput *> userInputs;

   std::vector<std::unique_ptr<PolyProxyModule>> polyProxies;
   std::vector<PolyProxyOutput *> proxyOutputs;
   std::vector<PolyProxyInput *> proxyInputs;
   std::vector<PolyProxyUserInput *> proxyUserInputs;

};

#endif //PATCHCORE_POLYMODULE_HPP
