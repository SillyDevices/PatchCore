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

package com.sillydevices.patchcore.modules.vco

import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.factory.ModuleParameter
import com.sillydevices.patchcore.module.factory.ModuleType
import com.sillydevices.patchcore.types.WaveForm

class LfoModule(name: String): FactoryModule(name, ModuleType.LFO) {

    override val parameters: List<ModuleParameter> = listOf(
        ModuleParameter("tune", 1f),
        ModuleParameter("lfo_mode", true)
    )

    val cv = registerInput("cv")
    val hz = registerInput("hz")
    val reset = registerInput("reset")
    val output = registerOutput("out")

    val userCv = registerFloatUserInput("cv", 0f..1f)
    val userHz = registerFloatUserInput("hz", 0f..200f)


    val pwm = registerInput("pwm")

    val userPwm = registerFloatUserInput("pwm", 0f..1f)
    val userWaveForm = registerEnumUserInput<WaveForm>("waveform")
    val userSingleShot = registerBooleanUserInput("single_shot")
}