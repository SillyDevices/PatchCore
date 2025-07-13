import com.vanniktech.maven.publish.AndroidSingleVariantLibrary
import com.vanniktech.maven.publish.JavadocJar
import com.vanniktech.maven.publish.KotlinMultiplatform
import com.vanniktech.maven.publish.SonatypeHost
import java.util.Properties

plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.vanniktech.maven.publish)
}

android {

    namespace = "com.sillydevices.patchcore.andoid"
    compileSdk = 35
    buildToolsVersion = "35.0.0"

    defaultConfig {
        minSdk = 21
        ndkVersion = libs.versions.ndk.get()
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        consumerProguardFiles.add(File("consumer-rules.pro"))
        externalNativeBuild {
            cmake {
                arguments("-DANDROID_STL=c++_shared")
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    externalNativeBuild {
        cmake {
            path = File("src/main/cpp/CMakeLists.txt")
            //available in github 3.10.2 3.18.1 3.22.1
//            version = "3.20.3"
        }
    }

    buildFeatures {
        prefabPublishing = true
    }

    prefab {
        create("PatchCore") {
            headers = "src/main/cpp/include"
        }
    }

    packaging {
        jniLibs {
            excludes.addAll(listOf("**/libPatchCore.so", "**/libc++_shared.so"))
        }
    }
}

val artifactVersion = "0.1.0"

mavenPublishing {
    publishToMavenCentral(SonatypeHost.CENTRAL_PORTAL, automaticRelease = false)
    signAllPublications()

    coordinates("com.sillydevices.patchcore", "native-android", artifactVersion)

    pom {
        name.set("PatchCore Android Native")
        description.set("Library for creating high-performance audio synthesizers in native Android applications using the PatchCore framework. This library is distributed via Prefab.")
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

    configure(
        AndroidSingleVariantLibrary(
            sourcesJar = true,
            publishJavadocJar = false,
        )
    )
}

task("getArtifactVersion") {
    doLast {
        val publication = publishing.publications.getByName("release") as MavenPublication
        println(publication.version)
    }
}