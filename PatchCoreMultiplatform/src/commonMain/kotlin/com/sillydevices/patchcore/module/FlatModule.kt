/*
 * PatchCore — Modular Synthesizer Engine
 * Copyright (c) 2026 Evgenii Petrov
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

package com.sillydevices.patchcore.module

import com.sillydevices.patchcore.context.PatchModuleContext
import com.sillydevices.patchcore.module.io.input.ModuleInput
import com.sillydevices.patchcore.module.io.output.ModuleOutput
import com.sillydevices.patchcore.module.io.user.UserInput

abstract class FlatModule(name: String): PatchModule(name, moduleNamePrefix = name) {
    // FlatModule like PatchModule, but did not create new context and new module instance
    // adds own modules to the parent

    override fun createFrom(parentContext: PatchModuleContext) {
        //can be called only from PatchModuleContext or PolyModuleContext
        //do not create new context and new module, just use the parent context
        applyContext(parentContext)
    }

    override fun applyPatchModuleContext(context: PatchModuleContext) {
        //all from PatchModule except patches
        applyContextForIO(context)
        applyChildrenSettings()
    }

    override fun addPatch(connection: Patch.Connection) {
        throw IllegalStateException("Not supported")
    }

    override fun removePatch(connection: Patch.Connection) {
        throw IllegalStateException("Not supported")
    }

    @Deprecated("not needed in FlatModule, use = instead")
    override fun expose(input: ModuleInput, withName: String?): ModuleInputDelegateProvider {
        return ModuleInputDelegateProvider(input, withName)
    }

    @Deprecated("not needed in FlatModule, use = instead")
    override fun expose(output: ModuleOutput, withName: String?): ModuleOutputDelegateProvider {
        return ModuleOutputDelegateProvider(output, withName)
    }

    @Deprecated("not needed in FlatModule, use = instead")
    override fun <T: UserInput> expose(userInput: T, withName: String?): ModuleUserInputDelegateProvider<T> {
        return ModuleUserInputDelegateProvider(userInput, withName)
    }
}