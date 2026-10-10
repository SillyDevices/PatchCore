import com.vanniktech.maven.publish.AndroidSingleVariantLibrary
import com.vanniktech.maven.publish.JavadocJar

plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.vanniktech.maven.publish)
}

// Only builds and packages native binaries. Kotlin JNI declarations stay in the KMP module.
android {
    namespace = "com.sillydevices.patchcore.android.nativeinterface"
    compileSdk = 35
    enableKotlin = false
    ndkVersion = libs.versions.ndk.get()

    defaultConfig {
        minSdk = 24
        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON"
                )
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
        }
    }
    buildFeatures {
        prefab = true
    }
    buildTypes {
        release {
            isMinifyEnabled = false
            packaging {
                jniLibs.keepDebugSymbols += "**/*.so"
            }
        }
    }
    // Owns libnativeinterface.so, libPatchCore.so and libc++_shared.so.
    // :PatchCore excludes its runtime libraries from the AAR but exposes them via Prefab.
}

dependencies {
    implementation(project(":PatchCore"))
    implementation(libs.oboe)
}

mavenPublishing {
    publishToMavenCentral(automaticRelease = false)
    signAllPublications()
    coordinates(
        "com.sillydevices.patchcore",
        "android-jni",
        libs.versions.artifacts.patchCoreMultiplatform.get()
    )
    pom {
        name.set("PatchCore Android JNI")
        description.set("Android JNI and audio binaries for PatchCore Kotlin Multiplatform.")
        inceptionYear.set("2025")
        url.set("https://github.com/SillyDevices/PatchCore")
        licenses {
            license {
                name.set("GNU Affero General Public License v3.0")
                url.set("https://www.gnu.org/licenses/agpl-3.0.en.html")
                distribution.set("repo")
            }
        }
        developers {
            developer {
                id.set("SillyDevices")
                name.set("Evgenii Petrov")
                url.set("https://github.com/SillyDevices")
            }
        }
        scm {
            url.set("https://github.com/SillyDevices/PatchCore")
            connection.set("scm:git:git://github.com:SillyDevices/PatchCore.git")
            developerConnection.set("scm:git:ssh://git@github.com:SillyDevices/PatchCore.git")
        }
    }
    configure(AndroidSingleVariantLibrary(javadocJar = JavadocJar.None()))
}
