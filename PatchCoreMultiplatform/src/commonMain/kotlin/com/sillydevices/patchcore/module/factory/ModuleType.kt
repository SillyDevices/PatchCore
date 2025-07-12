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

package com.sillydevices.patchcore.module.factory

public open class ModuleType(val value: String) {
    companion object
    object ADSREnvelope: ModuleType("ADSR")
    object FADEREnvelope: ModuleType("FADER")

    object BiQuadVcf: ModuleType("BIQUAD_VCF")
    object LFO: ModuleType("LFO")
    object VCO: ModuleType("VCO")

    object Attenuverter: ModuleType("ATTENUVERTER")
    object Const: ModuleType("CONST")
    object DCFilter: ModuleType("DCFILTER")
    object DelayLine: ModuleType("DELAYLINE")
    object Divider: ModuleType("DIVIDER")
    object HalfWaveRectifier: ModuleType("HALF_RECTIFIER")
    object Indicator: ModuleType("INDICATOR")
    object Inverse: ModuleType("INVERSE")
    object MixLinear: ModuleType("LINEAR_MIX")
    object MixLinearWithLimit: ModuleType("LINEAR_MIX_WITH_LIMIT")
    object MixCrossFader: ModuleType("MIX_CROSSFADE")
    object Noise: ModuleType("NOISE")
    object Pass: ModuleType("PASS")
    object PanCrossFade: ModuleType("PAN_CROSSFADE")
    object SampleAndHold: ModuleType("SAMPLE_AND_HOLD")
    object VCA: ModuleType("VCA")

    object Keyboard: ModuleType("KEYBOARD")
    object MultiKeyboard: ModuleType("MULTI_KEYBOARD")
}