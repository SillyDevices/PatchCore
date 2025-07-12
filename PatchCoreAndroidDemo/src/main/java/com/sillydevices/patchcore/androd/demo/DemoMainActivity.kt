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

package com.sillydevices.patchcore.androd.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Modifier
import com.sillydevices.patchcore.androd.demo.model.DemoMainActivityViewModel
import com.sillydevices.patchcore.androd.demo.ui.DemoVCOView
import com.sillydevices.patchcore.androd.demo.ui.theme.PatchCoreTheme

class DemoMainActivity : ComponentActivity() {

    val viewModel: DemoMainActivityViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            PatchCoreTheme {
                Scaffold() { paddingValues ->
                    Box(modifier = Modifier.fillMaxSize().safeDrawingPadding()) {
                        DemoVCOView(demoVcoInteractor = viewModel)
                    }
                }
            }
        }
    }

    override fun onStart() {
        super.onStart()
        viewModel.onStartAudio()
    }

    override fun onStop() {
        super.onStop()
        viewModel.onStopAudio()
    }

}