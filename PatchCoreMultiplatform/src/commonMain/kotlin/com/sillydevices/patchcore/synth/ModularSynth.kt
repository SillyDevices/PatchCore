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

package com.sillydevices.patchcore.synth

import com.sillydevices.patchcore.context.ModularSynthContext
import com.sillydevices.patchcore.context.ModuleContext
import com.sillydevices.patchcore.context.PatchCoreContext
import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.module.PatchModule

open class ModularSynth(): PatchModule("synth") {
    val monoOutput = registerInput("mono")
    val leftOutput = registerInput("left")
    val rightOutput = registerInput("right")


    override fun applyContext(context: ModuleContext) {
        context as? ModularSynthContext ?: throw IllegalArgumentException("Context must be of type ModularSynthContext not ${context::class.simpleName}")
        super.applyContext(context)
    }

    fun release() {
        val context = moduleContext as? PatchModuleContext
            ?: throw IllegalArgumentException("Context must be of type PatchModuleContext")
        context.release()
    }
}