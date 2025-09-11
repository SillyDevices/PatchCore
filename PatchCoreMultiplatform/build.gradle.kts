import com.vanniktech.maven.publish.AndroidSingleVariantLibrary
import com.vanniktech.maven.publish.JavadocJar
import com.vanniktech.maven.publish.KotlinMultiplatform
import com.vanniktech.maven.publish.SonatypeHost
import org.jetbrains.kotlin.gradle.dsl.JvmTarget
import java.util.Properties

plugins {
    alias(libs.plugins.kotlin.multiplatform)
    alias(libs.plugins.android.library)
    alias(libs.plugins.vanniktech.maven.publish)
}

kotlin {
    applyDefaultHierarchyTemplate()
    androidTarget {
        compilations.all {
            compileTaskProvider.configure {
                compilerOptions {
                    jvmTarget.set(JvmTarget.JVM_17)
                }
            }
        }
    }

    listOf(
        iosX64(),
        iosArm64(),
        iosSimulatorArm64()
    ).forEach {
        it.compilations.getByName("main") {
            cinterops {
                val libPatchCoreWrapper by creating {
                    // automatically loads .def from src/nativeInterop/cinterop/libPatchCoreWrapper.def
                    includeDirs("${layout.projectDirectory.asFile.absolutePath}/src/iosMain/objC/include")
                }
            }
        }
    }

    sourceSets {
        commonMain.dependencies {
            //put your multiplatform dependencies here
        }
        commonTest.dependencies {
            //implementation(libs.kotlin.test)
        }
    }
}

android {
    namespace = "com.sillydevices.patchcore"
    compileSdk = 33

    defaultConfig {
        minSdk = 24
        targetSdk = 33
        ndkVersion = libs.versions.ndk.get()

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        consumerProguardFiles("consumer-rules.pro")
        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON"
                )
                cppFlags("")
            }
        }
    }

    ndkVersion = libs.versions.ndk.get()

    externalNativeBuild {
        cmake {
            path = file("src/androidMain/cpp/CMakeLists.txt")
        }
    }

    buildFeatures {
        prefab = true
    }

    buildTypes {
        getByName("release") {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    packaging {
        jniLibs {
            // Main library, don't exclude anything from it
            // libPatchCore.so should be excluded from :PatchCore, it will be built here from prefab
            // NOTE!!! Other libraries that use PatchCoreMultiplatform should exclude libPatchCore.so
        }
    }
}

val artifactVersion = "0.1.4"

mavenPublishing {
    publishToMavenCentral(SonatypeHost.CENTRAL_PORTAL, automaticRelease = false)
    signAllPublications()

    coordinates("com.sillydevices.patchcore", "patchcore-multiplatform", artifactVersion)

    pom {
        name.set("PatchCore Kotlin Mutliplatform")
        description.set("Library for creatating modular audio synthesizers for Kotlin Multiplatform. Supports Android and iOS platforms.")
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

    KotlinMultiplatform(
        javadocJar = JavadocJar.None(),
        sourcesJar = true,
        androidVariantsToPublish = listOf("release")
    )
}

dependencies {
    //implementation(patchCoreWrapper.audio)
    //for developing purposes only
    implementation(project(":PatchCore"))

    implementation(libs.oboe)

    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("org.jetbrains.kotlinx:kotlinx-coroutines-test:1.7.3")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")

}

tasks.register("buildIosFramework") {
    // NOTE! set .library( type: .dynamic to build xcframework from swift package
    group = "build"
    description = "Builds PatchCore static libraries for iOS"

    val schemeName = "PatchCoreLibrary"
    val libraryName = "PatchCoreWrapper"
    val buildBaseDir = layout.buildDirectory.dir("xcode_build").get().asFile.absolutePath
    val outputFrameworkDir = layout.buildDirectory.dir("xcode_fw_output").get().asFile.absolutePath
    val outputDir = layout.buildDirectory.dir("xcode_output").get().asFile.absolutePath

    //TODO add inputs and outputs
    outputs.dir(outputDir)

    fun Task.createXArchive(
        configuration: String,
        destination: String,
    ) {
        exec {
            workingDir("../")
            val postfix = when (destination) {
                "generic/platform=iOS" -> "iphoneos"
                "generic/platform=iOS Simulator" -> "iphonesimulator"
                else -> throw IllegalArgumentException("Unsupported destination: $destination")
            }
            val command = mutableListOf(
                "xcrun", "xcodebuild", "archive",
                "-workspace", ".",
                "-scheme", schemeName,
                "-configuration", configuration,
                "-destination", destination,
                "-derivedDataPath", "${buildBaseDir}",
                "-archivePath", "${outputFrameworkDir}/${configuration}/${libraryName}-${postfix}.xcarchive",
                "-allowProvisioningUpdates",
                "SKIP_INSTALL=NO",
                "INSTALL_PATH=/Library/Frameworks",
                "BUILD_LIBRARY_FOR_DISTRIBUTION=YES"
            )
            if (configuration == "Debug") command.add("DEBUG_INFORMATION_FORMAT=dwarf-with-dsym")
            commandLine(command)
        }
    }

    fun Task.createXCFramework(
        configuration: String
    ) {
        exec {
            workingDir("../")
            val command = mutableListOf("xcodebuild", "-create-xcframework", "-output", "${outputDir}/${configuration}/${libraryName}.xcframework")
            listOf("-framework", "${outputFrameworkDir}/${configuration}/${libraryName}-iphoneos.xcarchive/Products/Library/Frameworks/${libraryName}.framework").forEach { command.add(it) }
            if (configuration == "Debug") {
                listOf("-debug-symbols", "${outputFrameworkDir}/${configuration}/${libraryName}-iphoneos.xcarchive/dSYMs/${libraryName}.framework.dSYM").forEach { command.add(it) }
            }
            listOf("-framework", "${outputFrameworkDir}/${configuration}/${libraryName}-iphonesimulator.xcarchive/Products/Library/Frameworks/${libraryName}.framework",).forEach { command.add(it) }
            if (configuration == "Debug") {
                listOf("-debug-symbols", "${outputFrameworkDir}/${configuration}/${libraryName}-iphonesimulator.xcarchive/dSYMs/${libraryName}.framework.dSYM").forEach { command.add(it) }
            }
            commandLine(command)
        }
    }

    doLast {
        createXArchive(configuration = "Release", destination = "generic/platform=iOS")
        createXArchive(configuration = "Release", destination = "generic/platform=iOS Simulator")
        //TODO delete otput directory if exists
        createXCFramework(configuration = "Release")
        createXArchive(configuration = "Debug", destination = "generic/platform=iOS")
        createXArchive(configuration = "Debug", destination = "generic/platform=iOS Simulator")
        //TODO delete otput directory if exists
        createXCFramework(configuration = "Debug")
    }
}
