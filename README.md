PatchCore is an open-source Kotlin Multiplatform library for building modular synthesizers and audio processing applications.

At its core, PatchCore is written in portable C++, enabling high-performance audio processing across platforms—from mobile and desktop to embedded systems. The Kotlin interface provides a modern, idiomatic API for composing and connecting audio modules in a flexible patching environment.

PatchCore is designed to let you build performant and agile synthesizers entirely in Kotlin, without needing to dive into C++ internals. At the same time, it offers deep flexibility: you can implement custom modules in C++ and expose them to Kotlin with just a few lightweight, template-based wrappers — or, in simple cases, with no additional code at all. 

---
# Example

```kotlin
class SoundEngine {

    class Synth: ModularSynth() {
        
        // PolyModule(name, polyphonyCount = 8)
        class Voice(name: String): PatchModule(name) { 
            val keyboard by module( KeyboardModule("keyboard") )

            val vco by module( VcoModule("vco") ) {
                userWaveForm.setValue(WaveForm.SQUARE)
                userPwm.setValue(0.5f)
            }

            val env by module(ADSREnvelopeModule("env") )
            val vca by module(VcaModule("vca"))

            val vcf by module( BiQuadVCFModule("vcf"))

            val lfo by module( LfoModule("lfo") )
            val pitchLfoAmount by module( AttenuverterModule("pitchAmount") )
            val cutoffLfoAmount by module( AttenuverterModule("cutoffAmount") )

            val cutoffEnvAmount by module( AttenuverterModule("cutoffEnvAmount") )

            val output = createOutput(vcf.output)

            override val defaultPatch = createPatch {
                keyboard.cv patchTo vco.cv
                vco.output patchTo vca.input

                keyboard.gate patchTo env.gate
                env.output patchTo vcf.input

                lfo.output patchTo pitchLfoAmount.input
                pitchLfoAmount.output patchTo vco.cv

                lfo.output patchTo cutoffLfoAmount.input
                cutoffLfoAmount.output patchTo vcf.cutoff

                env.output patchTo cutoffEnvAmount.input
                cutoffEnvAmount.output patchTo vcf.cutoff
            }
        }

        val voice by module( Voice("voice") )

        override val defaultPatch = createPatch {
            voice.output patchTo monoOutput
        }
    }

    private val patchCore = PatchCore()
    private val synth = patchCore.createSynth(Synth())
    private val audioInterface: AudioInterface = patchCore.createAudioInterface<AudioInterface>().apply {
        setOptions( createOptions() )
        setSynth(synth)
    }

    // should be tied to the lifecycle of the app
    fun start() {
        audioInterface.start()
    }

    // should be tied to the lifecycle of the app
    fun stop() {
        audioInterface.stop()
    }

    // keyboard events
    fun onKeyDown(note: Float) {
        synth.voice.keyboard.onPress(note)
    }

    fun onKeyUp(note: Float) {
        synth.voice.keyboard.onRelease(note)
    }

    // set parameters values of the synth
    fun setCutoffValue(cutoff: Float) {
        synth.voice.vcf.userCutoff.setValue(cutoff)
    }

    //...

}
```

---
# Setup
## Using as dependency
Add the dependency to your build.gradle.kts
```kotlin
// for kotlin multiplatform projects
commonMain.dependencies {
    implementation("com.sillydevices.patchcore:patchcore-multiplatform:0.1.5")
}
// for android projects
dependencies {
    implementation("com.sillydevices.patchcore:patchcore-multiplatform:0.1.5")
    // or
    implementation("com.sillydevices.patchcore:patchcore-multiplatform-android:0.1.5")
}

```

⚠️ Don’t forget to add a swift package dependency to your iOS project from https://github.com/SillyDevices/PatchCore


## Using for development as submodule
Initialize the Git submodule and add it to your project
```bash
git submodule init

git submodule add git@github.com:SillyDevices/PatchCore.git PatchCore

git submodule update --init --recursive PatchCore

cp local.properties PatchCore/local.properties
```

Then, include the submodule in your build setup:
settings.gradle.kts
```kotlin
includeBuild("PatchCore") {
    dependencySubstitution {
        substitute(module("com.sillydevices.patchcore:native-android")).using(project(":PatchCore"))
        substitute(module("com.sillydevices.patchcore:patchcore-multiplatform")).using(project(":PatchCoreMultiplatform"))
        substitute(module("com.sillydevices.patchcore:patchcore-multiplatform-android")).using(project(":PatchCoreMultiplatform"))
    }
}
```

add dependency to app module build.gradle.kts

```kotlin
//build.gradle.kts
dependencies {
    implementation("com.sillydevices.patchcore:patchcore-multiplatform:0.1.5")
}
```

add local swift package dependency to your iOS project from PatchCore folder