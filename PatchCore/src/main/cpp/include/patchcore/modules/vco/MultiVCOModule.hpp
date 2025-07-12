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

#ifndef PATCHCORE_MULTIVCO_MODULE_HPP
#define PATCHCORE_MULTIVCO_MODULE_HPP


#include "BaseVCO.hpp"
#include "patchcore/module/ModuleParameter.hpp"
#include "patchcore/module/input/user/BoolUserInput.hpp"

#include "patchcore/dsp/WaveForm.hpp"
#include "patchcore/module/input/user/EnumUserInput.hpp"

#include "patchcore/dsp/wavetable/mipmap/SawWaveTable.hpp"
#include "patchcore/dsp/wavetable/mipmap/TriangleWaveTable.hpp"
#include "patchcore/dsp/wavetable/mipmap/SquareWaveTable.hpp"
#include "patchcore/dsp/wavetable/WaveTableProvider.hpp"
#include "patchcore/dsp/osc/Noise.hpp"

#include "SuperWave.hpp"
#include "patchcore/module/output/LedOutput.hpp"

#define VCO_PARAMETER_TUNE "tune"
#define VCO_PARAMETER_LFO_MODE "lfo_mode"

#define VCO_INPUT_PWM "pwm"
#define VCO_INPUT_WAVEFORM "waveform"
#define VCO_INPUT_SINGLE_SHOT "single_shot"

class MultiVCOModule : public BaseVCO {
public:
    MultiVCOModule(std::string name,
                   int sampleRate,
                   WaveTableProvider *waveTableProvider,
                   const std::map<std::string, ModuleParameter>& parameters
    );
    MultiVCOModule(std::string name,
                   int sampleRate,
                   WaveTableProvider *waveTableProvider,
                   float tune = dsp::tune_C0,
                   bool lfoMode = false
    );
    MultiVCOModule(const MultiVCOModule& other);
    std::unique_ptr<Module> clone() const override;
    void init();
    virtual ~MultiVCOModule() = default;

public:
    void envelope() override;
    void setWaveForm(WaveForm waveform);

    WaveForm getCurrentWaveForm();

protected:
    WaveForm currentWaveForm = WaveForm::SAW;

    SawWaveTable *sawTable;
    TriangleWaveTable *triangleWaveTable;
    SquareWaveTable *squareWaveTable;
    SimpleSquareWaveTable *simpleSquareWaveTable;
    Noise *noise;
    SuperWave superSquare;
    SuperWave superSaw;

    bool lfoMode;

    LedOutput ledOutput = LedOutput("led", sampleRate);

    ModuleInput pwm = ModuleInput(VCO_INPUT_PWM);
    FloatUserInput userPwm = FloatUserInput(VCO_INPUT_PWM);
    EnumUserInput waveForm = EnumUserInput(VCO_INPUT_WAVEFORM);
    BoolUserInput singleShot = BoolUserInput(VCO_INPUT_SINGLE_SHOT);
};

#endif //PATCHCORE_MULTIVCO_MODULE_HPP
