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


/*
 * The main idea behind this class is to provide an PatchModule interface for polyphonic modules
 * it's acts like a container for multiple PatchModule instances, each representing a voice and proxies
 * methods to all voices
 */
class PolyModule : public PatchModule {
public:
    PolyModule(ModuleFactory *factory, std::string name, int sampleRate, int voiceCount);
    ~PolyModule() override;

    //Module interface
public:
    void onStartBlock(const BlockContext& context) override;
    void processSample(int sampleIndex) override;
    void processBlock() override;

    //PatchModule specific
    //creates a new input/output from existing ModuleInput/ModuleOutput
public:
    ExposedModuleInput* exposeInput(ModuleInput* input, const std::string& withName) override;
    ExposedModuleOutput* exposeOutput(ModuleOutput* output, const std::string& withName) override;
    virtual ExposedModuleOutput* exposeDemuxOutput(ModuleOutput* output, const std::string& withName, const int defaultVoiceIndex);
    //don't know in what case this is useful, but it is here for consistency
    UserInput* exposeUserInput(UserInput* input, const std::string& withName) override;
    //PatchModule specific
public:
    virtual Module* createModule(const std::string& moduleTypeName, const std::string& moduleName, const std::map<std::string, ModuleParameter>& parameters) override;
    virtual Module* addModule(std::unique_ptr<Module> module) override;
    virtual void clearModules() override;
    virtual Module* getModule(const std::string& moduleName) const override;
    //PatchModule Router specific
public:
    virtual void resetPatch() override;
    virtual void addPatch(ModuleOutput* output, ModuleInput* input) override;
    virtual void removePatch(ModuleOutput* output, ModuleInput* input) override;
    // polyphony support
public:
    size_t getVoiceCount() const;
    PatchModule* getVoice(size_t index);

    size_t getActiveVoiceCount() const;
    void setActiveVoiceCount(size_t count);

protected:
    std::vector<std::unique_ptr<PatchModule>> _voices;
    std::vector<PatchModule*> voices;
    size_t voiceCount;
    size_t activeVoiceCount;

   std::vector<std::unique_ptr<PolyProxyModule>> polyProxies;
   std::vector<PolyProxyOutput *> proxyOutputs;
   std::vector<PolyProxyInput *> proxyInputs;
   std::vector<PolyProxyUserInput *> proxyUserInputs;
   std::vector<ExposedModuleOutput *> exposedOutputs;
   std::vector<ExposedModuleInput *> exposedInputs;
   std::vector<ExposedModuleUserInput *> exposedUserInputs;
   std::vector<PolyProxyOutput *> ownedProxyOutputs;

};

#endif //PATCHCORE_POLYMODULE_HPP
