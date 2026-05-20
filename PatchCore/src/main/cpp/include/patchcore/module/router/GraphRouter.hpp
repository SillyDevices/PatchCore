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

#ifndef PATCHCORE_TOPOROUTER_HPP
#define PATCHCORE_TOPOROUTER_HPP

#include "AbstractRouter.hpp"

#include <mutex>
#include <vector>
#include <unordered_map>

class GraphRouterDiagnostics;

class GraphRouter: public AbstractRouter {
public:
    explicit GraphRouter(Module* parent);
    virtual ~GraphRouter() = default;

    void addModule(Module *module) override;
    void removeModule(Module *module) override;
    void clearModules() override;

    //TODO rename to addPatch/removePatch/clearPatches
    void add(ModuleOutput* from, ModuleInput* to) override;
    void remove(ModuleOutput* from, ModuleInput* to) override;
    void reset() override;

    void moduleInputChanged(Module* module) override;

    void onStartBlock(const BlockContext& context) override;
    void processSample(int sampleIndex);
    void processBlock();

    [[nodiscard]]
    std::vector<std::pair<ModuleOutput*, ModuleInput*>> getPatches() const override;

    bool containLoops() const { return _containLoops; }

private:
    friend class GraphRouterDiagnostics;

protected:
    void updateModuleGraph();


protected:
    Module* parentModule;

    std::vector<Module*> modules;
    std::vector<std::pair<ModuleOutput*, ModuleInput*>> patches;

    // unique ids assigned to modules
    std::unordered_map<Module*, int> moduleToId;
    std::unordered_map<int, Module*> idToModule;
    int nextModuleId = 0;

    struct EnvelopeStage {
        struct SampleInputRoute {
            ModuleInput* input = nullptr;
            std::vector<ModuleOutput*> outputs;
            std::vector<bool> readPreviousSample;
        };

        struct SampleModuleRoute {
            Module* module = nullptr;
            std::vector<SampleInputRoute> inputRoutes;
        };

        std::vector<Module*> modulesInStage;
        std::vector<ModuleInput*> inputsInStage;
        std::vector<std::vector<ModuleOutput*>> outputsInStage;
        std::vector<SampleModuleRoute> sampleModuleRoutes;
        bool hasLoop = false;
    };

    void processStageSampleWithLoop(const EnvelopeStage& stage, int sampleIndex);
    void processStageSample(const EnvelopeStage& stage, int sampleIndex);
    void processStageBlock(const EnvelopeStage& stage);

    std::vector<ModuleInput*> allEnvelopeInputs;
    std::vector<FloatUserInput*> allEnvelopeUserInputs;
    std::vector<Module*> allEnvelopeModules;
    std::vector<EnvelopeStage> envelopeStages;

    bool _containLoops = false;
    std::mutex updateModuleGraphMutex;

};

#endif //PATCHCORE_TOPOROUTER_HPP
