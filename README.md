PatchCore is an open-source Kotlin Multiplatform library for building modular synthesizers and audio processing applications.

At its core, PatchCore is written in portable C++, enabling high-performance audio processing across platforms—from mobile and desktop to embedded systems. The Kotlin interface provides a modern, idiomatic API for composing and connecting audio modules in a flexible patching environment.

PatchCore is designed to let you build performant and agile synthesizers entirely in Kotlin, without needing to dive into C++ internals. At the same time, it offers deep flexibility: you can implement custom modules in C++ and expose them to Kotlin with just a few lightweight, template-based wrappers — or, in simple cases, with no additional code at all. 

---
# Setup
## Using as dependency
Add the dependency to your build.gradle.kts
```kotlin
// for kotlin multiplatform projects
commonMain.dependencies {
    implementation("com.sillydevices.patchcore:patchcore-multiplatform:0.1.0")
}
// for android projects
dependencies {
    implementation("com.sillydevices.patchcore:patchcore-multiplatform:0.1.0")
    // or
    implementation("com.sillydevices.patchcore:patchcore-multiplatform-android:0.1.0")
}

```

⚠️ Don’t forget to add a swift package dependency to your iOS project.

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
includeBuild("PatchCore")
```

add dependency to app module build.gradle.kts and enable prefab

```kotlin
//build.gradle.kts
dependencies {
    implementation("com.sillydevices:PatchCoreMultiplatform:1.0-DEV")
}
```
