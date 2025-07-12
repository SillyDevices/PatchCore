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

#ifndef PATCHCORE_DEFAULTMODULEFACTORY_HPP
#define PATCHCORE_DEFAULTMODULEFACTORY_HPP

#include "ModuleFactory.hpp"
#include "patchcore/modules/vco/MultiVCOModule.hpp"
#include "patchcore/dsp/wavetable/WaveTableProvider.hpp"
#include "patchcore/dsp/wavetable/DefaultWaveTableProvider.hpp"

//modules
#include "patchcore/modules/envelope/ADSREnvelopeModule.hpp"
#include "patchcore/modules/envelope/FadeEnvelopeModule.hpp"
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
#include "patchcore/modules/input/event/keyboard/KeyboardModule.hpp"
#include "patchcore/modules/input/event/keyboard/SingleKeyboardModule.hpp"


#define ADSR_ENVELOPE_MODULE_TYPE_NAME          "ADSR"
#define FADE_ENVELOPE_MODULE_TYPE_NAME          "FADER"

#define KEYBOARD_MODULE_TYPE_NAME               "KEYBOARD"
#define MULTI_KEYBOARD_MODULE_TYPE_NAME         "MULTI_KEYBOARD"

#define DATTORRO_REVERB_MODULE_TYPE_NAME        "DATTORRO"
#define FREEVERB_REVERB_MODULE_TYPE_NAME        "FREEVERB"
#define BIQUAD_VCF_MODULE_TYPE_NAME             "BIQUAD_VCF"
#define LFO_MODULE_TYPE_NAME                    "LFO"
#define VCO_MODULE_TYPE_NAME                    "VCO"

#define ATTENUVERTER_MODULE_TYPE_NAME           "ATTENUVERTER"
#define CONST_MODULE_TYPE_NAME                  "CONST"
#define DCFILTER_MODULE_TYPE_NAME               "DCFILTER"
#define DELAYLINE_MODULE_TYPE_NAME              "DELAYLINE"
#define DIVIDER_MODULE_TYPE_NAME                "DIVIDER"
#define HALFWAVE_RECTIFIER_MODULE_TYPE_NAME     "HALF_RECTIFIER"
#define INDICATOR_MODULE_TYPE_NAME              "INDICATOR"
#define INVERSE_MODULE_TYPE_NAME                "INVERSE"
#define LINEAR_MIX_MODULE_TYPE_NAME             "LINEAR_MIX"
#define LINEAR_MIX_WITH_LIMIT_MODULE_TYPE_NAME  "LINEAR_MIX_WITH_LIMIT"
#define MIX_CROSSFADE_MODULE_TYPE_NAME          "MIX_CROSSFADE"
#define NOISE_MODULE_TYPE_NAME                  "NOISE"
#define PASS_MODULE_TYPE_NAME                   "PASS"
#define PAN_CROSSFADE_MODULE_TYPE_NAME          "PAN_CROSSFADE"
#define SAMPLE_AND_HOLD_MODULE_TYPE_NAME        "SAMPLE_AND_HOLD"
#define VCA_MODULE_TYPE_NAME                    "VCA"


class DefaultModuleFactory: public ModuleFactory {
public:
    DefaultModuleFactory(WaveTableProvider* waveTableProvider, ModuleFactory* customModuleFactory);
    virtual ~DefaultModuleFactory() = default;
public:
    std::unique_ptr<Module> createModule(const std::string& moduleTypeName, const std::string& moduleName, int sampleRate, const std::map<std::string, ModuleParameter>& parameters) override;
private:
    //TODO remake to smart pointer! maybe?
    WaveTableProvider* waveTableProvider;
    ModuleFactory* customModuleFactory;
};

#endif //PATCHCORE_DEFAULTMODULEFACTORY_HPP
