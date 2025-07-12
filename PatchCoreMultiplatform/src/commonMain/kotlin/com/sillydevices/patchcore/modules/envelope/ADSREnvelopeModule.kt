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

package com.sillydevices.patchcore.modules.envelope

import com.sillydevices.patchcore.module.FactoryModule
import com.sillydevices.patchcore.module.factory.ModuleType

class ADSREnvelopeModule(name: String): FactoryModule(name, ModuleType.ADSREnvelope) {
    val gate = registerInput("gate")
    val sustain = registerInput("s")

    val output = registerOutput("out")
    val outExp = registerOutput("exp")

    val userDelay = registerFloatUserInput("dl", 0f..1f)
    val userAttack = registerFloatUserInput("a", 0f..1f)
    val userDecay = registerFloatUserInput("d", 0f..5f)
    val userSustain = registerFloatUserInput("s", 0f..1f)
    val userRelease = registerFloatUserInput("r", 0f..5f)
    val userExpCurve = registerFloatUserInput("curve_exp", -1f..1f)
    val userCycle = registerBooleanUserInput("c")

}