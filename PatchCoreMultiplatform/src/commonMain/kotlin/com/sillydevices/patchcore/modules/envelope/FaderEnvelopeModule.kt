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
import com.sillydevices.patchcore.module.factory.ModuleParameter
import com.sillydevices.patchcore.module.factory.ModuleType

class FaderEnvelopeModule(name: String = "fader", time: Float = 0.3f, isOpened: Boolean = true): FactoryModule(name, ModuleType.FADEREnvelope) {
    override val parameters: List<ModuleParameter> = listOf(
        ModuleParameter("time", time),
        ModuleParameter("isOpened", isOpened)
    )

    val output = registerOutput("out")

    val gate = registerFloatUserInput("gate", 0f..1f)
}