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

#include <vector>
#include <unordered_map>

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

    void onStartBuffer(int size) override;
    void envelope() override;

    [[nodiscard]]
    std::vector<std::pair<ModuleOutput*, ModuleInput*>> getPatches() const override;

    bool containLoops() const { return _containLoops; }

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
        std::vector<Module*> modulesInStage;
        std::vector<ModuleInput*> inputsInStage;
        std::vector<std::vector<ModuleOutput*>> outputsInStage;
    };

    std::vector<ModuleInput*> allEnvelopeInputs;
    std::vector<FloatUserInput*> allEnvelopeUserInputs;
    std::vector<Module*> allEnvelopeModules;
    std::vector<EnvelopeStage> envelopeStages;

    bool _containLoops = false;

};

#endif //PATCHCORE_TOPOROUTER_HPP
