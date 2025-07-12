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

#include "patchcore/modules/vco/MultiVCOModule.hpp"
#include "patchcore/dsp/osc/NativeWaveOsc.hpp"
#include "patchcore/module/output/LedOutput.hpp"
#include "patchcore/dsp/waveshaper/SimpleWaveShaper.hpp"

MultiVCOModule::MultiVCOModule(std::string name, int sampleRate, WaveTableProvider *waveTableProvider, const std::map<std::string, ModuleParameter>& parameters)
    : MultiVCOModule(name, sampleRate, waveTableProvider,
                     ModuleParameter::getFloatOrDefault(parameters, VCO_PARAMETER_TUNE, dsp::tune_C0),
                     ModuleParameter::getBoolOrDefault(parameters, VCO_PARAMETER_LFO_MODE, false)) {
}

MultiVCOModule::MultiVCOModule(
        std::string name,
        int sampleRate,
        WaveTableProvider *waveTableProvider,
        float tune,
        bool lfoMode
) : BaseVCO(name, sampleRate, tune),
    sawTable(waveTableProvider->getSawWaveTable()),
    triangleWaveTable(waveTableProvider->getTriangleWaveTable()),
    squareWaveTable(waveTableProvider->getSquareWaveTable()),
    simpleSquareWaveTable(waveTableProvider->getSimpleSquareWaveTable()),
    superSquare(SuperWave(simpleSquareWaveTable, sampleRate)),
    superSaw(SuperWave(sawTable, sampleRate)),
    lfoMode(lfoMode){
    init();
    //TODO noise
}


MultiVCOModule::MultiVCOModule(const MultiVCOModule& other)
    : BaseVCO(other.name, other.sampleRate, other.tune),
      sawTable(other.sawTable),
      triangleWaveTable(other.triangleWaveTable),
      squareWaveTable(other.squareWaveTable),
      simpleSquareWaveTable(other.simpleSquareWaveTable),
      superSquare(other.superSquare),
      superSaw(other.superSaw),
      lfoMode(other.lfoMode)
{
    init();
    currentWaveForm = other.currentWaveForm;
    copyIOs(other);
}

std::unique_ptr<Module> MultiVCOModule::clone() const {
    return std::make_unique<MultiVCOModule>(*this);
}

void MultiVCOModule::init() {
    registerInput(pwm);
    registerUserInput(userPwm);
    registerUserInput(waveForm);
    registerUserInput(singleShot);
    registerOutput(ledOutput);
}

void MultiVCOModule::envelope() {
    BaseVCO::envelope();
    auto pwmValue = pwm.value + userPwm.value;
    auto cvValue = cv.value + userCv.value;
    auto fm = hz.value;
    if (!lfoMode) {
        fm = hz.value * 100.0f;
    }
    auto hzValue = userHz.value + fm;
    auto waveShaperCv = pwmValue - 0.5f;

    // old code lfo userHz to frequency relation
    // 1 hz = 1hz
    // 10 hz = 19hz
    // 20 hz = 39hz
    // 100 hz = 199hz
    // 200 hz = 399hz
    // x hz = 2x-1
    // definitely wrong. i need to compensate it for all users presets

    auto frequency = 0.0f;
    if (!lfoMode) {
        frequency = computeFrequency(cvValue) + hzValue;
    } else {
        frequency = computeFrequency(cvValue + log2f(hzValue));
    }
    frequency = std::min(maxFrequency, frequency);

    float result = 0.0f;
    currentWaveForm = static_cast<WaveForm>(waveForm.getValue());
    switch (currentWaveForm) {
        case WaveForm::SAW:
            result = sawTable->getSample(frequency, phase);
            result = simpleWaveShaper(result, waveShaperCv);;
            break;
        case WaveForm::SUPER_SAW:
            result = superSaw.envelope(frequency, pwmValue);
            break;
        case WaveForm::RAMP:
            result = -1.0f * sawTable->getSample(frequency, phase);
            result = simpleWaveShaper(result, waveShaperCv);
            break;
        case WaveForm::SQUARE:
            result = squareWaveTable->getSample(frequency, pwmValue, phase);
            break;
        case WaveForm::SUPER_SQUARE:
            result = superSquare.envelope(frequency, pwmValue);
            break;
        case WaveForm::TRIANGLE:
            result = triangleWaveTable->getSample(frequency, phase);
            result = simpleWaveShaper(result, waveShaperCv);
            break;
        case WaveForm::SIN:
            result = nativeSin(phase);
            result = simpleWaveShaper(result, waveShaperCv);
            break;
        case WaveForm::NOISE:
            result = noise->envelope();
            break;
    }
//    result = result + syncBlep.process();
    output.value = result;
    if (lfoMode) {
        ledOutput.value = result;
        ledOutput.envelope();
    }
//    lastValue = output.value;
    phaseIncrement(frequency);
}

void MultiVCOModule::setWaveForm(WaveForm waveform) {
    currentWaveForm = waveform;
}

WaveForm MultiVCOModule::getCurrentWaveForm() {
    return currentWaveForm;
}
