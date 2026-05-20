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

#ifndef PATCHCORE_BIQUADVCF_MODULE_HPP
#define PATCHCORE_BIQUADVCF_MODULE_HPP

#include "VCF.hpp"
#include "patchcore/module/ModuleParameter.hpp"

#define BIQUAD_VCF_PARAMETER_OUTPUT_GAIN "output_gain"

#define BIQUAD_VCF_INPUT_GAIN "gain"


//TODO add parameters to setup default cuttof, resonance and type

class BiQuadVCFModule: public VCF {
public:
    enum Type {
        LOWPASS_1POLE,
        HIGHPASS_1POLE,
        BANDPASS_1POLE,
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOTCH,
        LOWSHELF,
        PEAKING,
        HIGHSHELF,
        ALLPASS
    };
public:
    BiQuadVCFModule(std::string name, int sampleRate, std::map<std::string, ModuleParameter> parameters);
    BiQuadVCFModule(std::string name, float outputGain, int sampleRate);
    BiQuadVCFModule(const BiQuadVCFModule& other);
    std::unique_ptr<Module> clone() const override;
    virtual ~BiQuadVCFModule() = default;
    void init();
public:
    void processSample(int sampleIndex) override;
    void processBlock() override;
protected:
    inline void processSampleInline(int sampleIndex);
    void computeFilter(float cutoff, float resonance, float gainDB = 0.f);
    void reset();
protected:
    float b[3] = {0, 0 ,0};
    float a[2] = {0, 0};
    float x[2] = {0, 0};
    float y[2] = {0, 0};
protected:
    Type type = LOWPASS;
    float outputGain = 0.5f;

    FloatUserInput userGain = FloatUserInput(BIQUAD_VCF_INPUT_GAIN);
};

#endif //PATCHCORE_BIQUADVCF_MODULE_HPP
