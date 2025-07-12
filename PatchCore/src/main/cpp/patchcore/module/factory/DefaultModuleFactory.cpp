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

#include "patchcore/dsp/wavetable/DefaultWaveTableProvider.hpp"
#include "patchcore/module/factory/DefaultModuleFactory.hpp"
//modules
#include "patchcore/modules/envelope/ADSREnvelopeModule.hpp"
#include "patchcore/modules/envelope/FadeEnvelopeModule.hpp"

#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include "patchcore/modules/input/event/keyboard/SingleKeyboardModule.hpp"

#include "patchcore/modules/reverb/FreeverbReverbModule.hpp"
#include "patchcore/modules/reverb/DattorroReverbModule.hpp"
#include "patchcore/modules/vcf/BiQuadVCFModule.hpp"
#include "patchcore/modules/vco/MultiVCOModule.hpp"

#include "patchcore/modules/AttenuverterModule.hpp"
#include "patchcore/modules/ConstModule.hpp"
#include "patchcore/modules/DCFilterModule.hpp"
#include "patchcore/modules/DelayLineModule.hpp"
#include "patchcore/modules/DividerModule.hpp"
#include "patchcore/modules/HalfRectifierModule.hpp"
#include "patchcore/modules/IndicatorModule.hpp"
#include "patchcore/modules/InverseModule.hpp"
#include "patchcore/modules/LinearMixModule.hpp"
#include "patchcore/modules/LinearMixWithLimitModule.hpp"
#include "patchcore/modules/MixCrossFadeModule.hpp"
#include "patchcore/modules/NoiseModule.hpp"
#include "patchcore/modules/PassModule.hpp"
#include "patchcore/modules/PanCrossFadeModule.hpp"
#include "patchcore/modules/SampleAndHoldModule.hpp"
#include "patchcore/modules/VCAModule.hpp"


DefaultModuleFactory::DefaultModuleFactory(WaveTableProvider *waveTableProvider, ModuleFactory* customModuleFactory) : ModuleFactory(), waveTableProvider(waveTableProvider), customModuleFactory(customModuleFactory) {
}

//DefaultModuleFactory::DefaultModuleFactory(int sampleRate, ModuleFactory* customModuleFactory) : ModuleFactory() {
//    ownWaveTableProvider = std::make_shared<DefaultWaveTableProvider>(sampleRate);
//    waveTableProvider = ownWaveTableProvider.get();
//}

std::unique_ptr<Module> DefaultModuleFactory::createModule(const std::string& moduleTypeName, const std::string& moduleName, int sampleRate, const std::map<std::string, ModuleParameter>& parameters) {
    if (customModuleFactory != nullptr){
        auto customModule = customModuleFactory->createModule(moduleTypeName, moduleName, sampleRate, parameters);
        if (customModule != nullptr) {
            return customModule;
        }
    }
    if (moduleTypeName == ATTENUVERTER_MODULE_TYPE_NAME) {
        return std::make_unique<AttenuverterModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == ADSR_ENVELOPE_MODULE_TYPE_NAME) {
        return std::make_unique<ADSREnvelopeModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == FADE_ENVELOPE_MODULE_TYPE_NAME) {
        return std::make_unique<FadeEnvelopeModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == KEYBOARD_MODULE_TYPE_NAME) {
        return std::make_unique<SingleKeyboardModule>(moduleName, sampleRate);
    } else if (moduleTypeName == MULTI_KEYBOARD_MODULE_TYPE_NAME) {
        return std::make_unique<KeyboardModule>(moduleName, sampleRate);
    } else if (moduleTypeName == DATTORRO_REVERB_MODULE_TYPE_NAME) {
        return std::make_unique<DattorroReverbModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == FREEVERB_REVERB_MODULE_TYPE_NAME) {
        return std::make_unique<FreeverbReverbModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == BIQUAD_VCF_MODULE_TYPE_NAME) {
        return std::make_unique<BiQuadVCFModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == VCO_MODULE_TYPE_NAME) {
        return std::make_unique<MultiVCOModule>(moduleName, sampleRate, waveTableProvider, parameters);
    } else if (moduleTypeName == LFO_MODULE_TYPE_NAME) {
        return std::make_unique<MultiVCOModule>(moduleName, sampleRate, waveTableProvider, parameters);
    } else if (moduleTypeName == CONST_MODULE_TYPE_NAME) {
        return std::make_unique<ConstModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == DCFILTER_MODULE_TYPE_NAME) {
        return std::make_unique<DCFilterModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == DELAYLINE_MODULE_TYPE_NAME) {
        return std::make_unique<DelayLineModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == DIVIDER_MODULE_TYPE_NAME) {
        return std::make_unique<DividerModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == HALFWAVE_RECTIFIER_MODULE_TYPE_NAME) {
        return std::make_unique<HalfRectifierModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == INDICATOR_MODULE_TYPE_NAME) {
        return std::make_unique<IndicatorModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == INVERSE_MODULE_TYPE_NAME) {
        return std::make_unique<InverseModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == LINEAR_MIX_MODULE_TYPE_NAME) {
        return std::make_unique<LinearMixModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == LINEAR_MIX_WITH_LIMIT_MODULE_TYPE_NAME) {
        return std::make_unique<LinearMixWithLimitModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == MIX_CROSSFADE_MODULE_TYPE_NAME) {
        return std::make_unique<MixCrossFadeModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == NOISE_MODULE_TYPE_NAME) {
        return std::make_unique<NoiseModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == PASS_MODULE_TYPE_NAME) {
        return std::make_unique<PassModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == PAN_CROSSFADE_MODULE_TYPE_NAME) {
        return std::make_unique<PanCrossFadeModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == SAMPLE_AND_HOLD_MODULE_TYPE_NAME) {
        return std::make_unique<SampleAndHoldModule>(moduleName, sampleRate, parameters);
    } else if (moduleTypeName == VCA_MODULE_TYPE_NAME) {
        return std::make_unique<VCAModule>(moduleName, sampleRate, parameters);
    }
    return nullptr;
}
