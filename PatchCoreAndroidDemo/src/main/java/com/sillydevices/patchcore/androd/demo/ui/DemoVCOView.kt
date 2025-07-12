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

package com.sillydevices.patchcore.androd.demo.ui

import android.util.Log
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.IconButton
import androidx.compose.material3.IconToggleButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.sillydevices.patchcore.androd.demo.ui.theme.PatchCoreTheme
import com.sillydevices.patchcore.types.WaveForm
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow


interface DemoVcoInteractor {
    val activeCb: Flow<Unit>
    val vcoCV: StateFlow<Float>
    val vcoHZ: StateFlow<Float>
    val vcoWaveform: StateFlow<WaveForm>
    val vcoPWM: StateFlow<Float>

    fun setVcoCV(value: Float)
    fun setVcoHZ(value: Float)
    fun setVcoPWM(value: Float)
    fun setVcoWaveform(value: WaveForm)

    val vcfCutoff: StateFlow<Float>
    fun setVcfCutoff(value: Float)

    val vcfResonance: StateFlow<Float>
    fun setVcfResonance(value: Float)

    val bypassEnvelope: StateFlow<Boolean>
    fun setBypassEnvelope(value: Boolean)
    fun onPressEnvelope(note: Float)
    fun onReleaseEnvelope(note: Float)

    val muted: StateFlow<Boolean>
    fun setMuted(value: Boolean)

    val volume: StateFlow<Float>
    fun setVolume(value: Float)
}

@Composable
fun DemoVCOView(
    demoVcoInteractor: DemoVcoInteractor
) {
    val active = demoVcoInteractor.activeCb.collectAsStateWithLifecycle(Unit)
    Column(modifier = Modifier.fillMaxSize().padding(horizontal = 16.dp, vertical = 8.dp)) {
        Text("VCO Parameters")
        val vcoCv = demoVcoInteractor.vcoCV.collectAsStateWithLifecycle()
        Spacer(Modifier.height(8.dp))
        Text("cv: ${vcoCv.value}", style = MaterialTheme.typography.bodySmall)
        Slider(
            value = vcoCv.value,
            onValueChange = { demoVcoInteractor.setVcoCV(it) },
            valueRange = 0f..10f,
            modifier = Modifier.fillMaxWidth()
        )
        val vcoHz = demoVcoInteractor.vcoHZ.collectAsStateWithLifecycle()
        Spacer(Modifier.height(8.dp))
        Text("hz: ${vcoHz.value}", style = MaterialTheme.typography.bodySmall)
        Slider(
            value = vcoHz.value,
            onValueChange = { demoVcoInteractor.setVcoHZ(it) }, // Assuming setVcoCV also sets Hz
            valueRange = 0f..1000f, // Adjust range as needed
            modifier = Modifier.fillMaxWidth()
        )
        val vcoPwm = demoVcoInteractor.vcoPWM.collectAsStateWithLifecycle()
        Spacer(Modifier.height(8.dp))
        Text("pwm: ${vcoPwm.value}", style = MaterialTheme.typography.bodySmall)
        Slider(
            value = vcoPwm.value,
            onValueChange = { demoVcoInteractor.setVcoPWM(it) },
            valueRange = 0f..1f,
            modifier = Modifier.fillMaxWidth()
        )
        val vcoWaveform = demoVcoInteractor.vcoWaveform.collectAsStateWithLifecycle()
        Spacer(Modifier.height(8.dp))
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("waveform:")
            Spacer(Modifier.width(16.dp))
            var isDropDownVisible: Boolean by remember { mutableStateOf(false) }
            Button(onClick = { isDropDownVisible = !isDropDownVisible }) {
                Text("${vcoWaveform.value.name}")
                DropdownMenu(expanded = isDropDownVisible, onDismissRequest = { isDropDownVisible = false }) {
                    WaveForm.entries.filter { it != WaveForm.NOISE }.forEach { waveform ->
                        DropdownMenuItem(
                            onClick = {
                                demoVcoInteractor.setVcoWaveform(waveform)
                                isDropDownVisible = false
                            },
                            text = { Text(waveform.name) }
                        )
                    }
                }
            }
        }
        Text("VCF Parameters")
        val vcfCutoff = demoVcoInteractor.vcfCutoff.collectAsStateWithLifecycle()
        Text("cutoff: ${vcfCutoff.value}", style = MaterialTheme.typography.bodySmall)
        Slider(
            value = vcfCutoff.value,
            onValueChange = { demoVcoInteractor.setVcfCutoff(it) }, // Assuming setVcoCV also sets Hz
            valueRange = 0f..10f, // Adjust range as needed
            modifier = Modifier.fillMaxWidth()
        )
        val vcfResonance = demoVcoInteractor.vcfResonance.collectAsStateWithLifecycle()
        Spacer(Modifier.height(8.dp))
        Text("resonance: ${vcfResonance.value}", style = MaterialTheme.typography.bodySmall)
        Slider(
            value = vcfResonance.value,
            onValueChange = { demoVcoInteractor.setVcfResonance(it) },
            valueRange = 0f..1f,
            modifier = Modifier.fillMaxWidth()
        )

        Spacer(Modifier.height(8.dp))
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("Toggle env:")
            Box {
                val source = remember { MutableInteractionSource() }
                val pressed = source.collectIsPressedAsState()
                LaunchedEffect(pressed.value) {
                    if (pressed.value) {
                        Log.d("DemoVCOView", "Envelope button pressed")
                        demoVcoInteractor.onPressEnvelope(1f)
                    } else {
                        Log.d("DemoVCOView", "Envelope button released")
                        demoVcoInteractor.onReleaseEnvelope(1f)
                    }
                }
                IconButton(interactionSource = source, onClick = {}) {
                    Text("1")
                }
            }
            Spacer(Modifier.width(16.dp))
            Box {
                val source = remember { MutableInteractionSource() }
                val pressed = source.collectIsPressedAsState()
                LaunchedEffect(pressed.value) {
                    if (pressed.value) {
                        Log.d("DemoVCOView", "Envelope button pressed")
                        demoVcoInteractor.onPressEnvelope(2f)
                    } else {
                        Log.d("DemoVCOView", "Envelope button released")
                        demoVcoInteractor.onReleaseEnvelope(2f)
                    }
                }
                IconButton(interactionSource = source, onClick = {}) {
                    Text("2")
                }
            }
            Spacer(Modifier.width(24.dp))
            Text("Bypass")
            Spacer(Modifier.width(16.dp))
            val bypassEnvelope = demoVcoInteractor.bypassEnvelope.collectAsStateWithLifecycle()
            Switch(
                checked = bypassEnvelope.value,
                onCheckedChange = { demoVcoInteractor.setBypassEnvelope(!bypassEnvelope.value) }
            )
        }
        Spacer(Modifier.height(8.dp))
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("Volume:")
            Spacer(Modifier.width(16.dp))
            val volume = demoVcoInteractor.volume.collectAsStateWithLifecycle()
            Slider(
                value = volume.value,
                onValueChange = { demoVcoInteractor.setVolume(it) },
                valueRange = 0f..2f,
                modifier = Modifier.weight(1f)
            )
            Spacer(Modifier.width(16.dp))
            val muted = demoVcoInteractor.muted.collectAsStateWithLifecycle()
            Switch(checked = muted.value, onCheckedChange =  {demoVcoInteractor.setMuted(!muted.value) })
        }
    }
}

@Preview
@Composable
fun DemoVCOViewPreview() {
    PatchCoreTheme {
        val interactor = DemoVCOInteractorMock()
        DemoVCOView(interactor)
    }
}