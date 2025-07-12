cmake_minimum_required(VERSION 3.6)

set (CMAKE_CXX_STANDARD 17)

project("patchcore" VERSION 1.0.0 LANGUAGES CXX)

include_directories(include)

add_library(
        PatchCore # Name of the library
        SHARED # Type of the library, can be STATIC or SHARED
        # Source files
        patchcore/module/Module.cpp
        patchcore/module/PatchModule.cpp
        patchcore/module/PolyModule.cpp
        patchcore/module/PolyProxyModule.cpp
        patchcore/module/Router.cpp

        patchcore/module/ModuleParameter.cpp

        patchcore/module/input/ModuleInput.cpp
        patchcore/module/output/ModuleOutput.cpp
        patchcore/module/input/module/TriggerModuleInput.cpp
        patchcore/module/input/user/FloatUserInput.cpp
        patchcore/module/input/user/EnumUserInput.cpp
        patchcore/module/input/user/BoolUserInput.cpp
        patchcore/module/input/user/special/PassFloatUserInput.cpp
        patchcore/module/input/user/special/ProxyFloatUserInput.cpp
        patchcore/module/input/user/special/OnStartBufferCallbackUserInput.cpp
        patchcore/module/input/user/special/CallbackFloatUserInput.cpp
        patchcore/module/input/user/special/CallbackBooleanUserInput.cpp

        patchcore/module/factory/DefaultModuleFactory.cpp

        patchcore/modules/envelope/ADSREnvelopeModule.cpp
        patchcore/modules/envelope/BaseEnvelope.cpp
        patchcore/modules/envelope/FadeEnvelopeModule.cpp
        patchcore/modules/reverb/FreeverbReverbModule.cpp
        patchcore/modules/reverb/DattorroReverbModule.cpp
        patchcore/modules/vcf/BiQuadVCFModule.cpp
        patchcore/modules/vcf/VCF.cpp
        patchcore/modules/vco/BaseVCO.cpp
        patchcore/modules/vco/MultiVCOModule.cpp

        patchcore/modules/AttenuverterModule.cpp
        patchcore/modules/ConstModule.cpp
        patchcore/modules/DCFilterModule.cpp
        patchcore/modules/DelayLineModule.cpp
        patchcore/modules/DividerModule.cpp
        patchcore/modules/HalfRectifierModule.cpp
        patchcore/modules/IndicatorModule.cpp
        patchcore/modules/InverseModule.cpp
        patchcore/modules/LinearMixModule.cpp
        patchcore/modules/LinearMixWithLimitModule.cpp
        patchcore/modules/MixCrossFadeModule.cpp
        patchcore/modules/NoiseModule.cpp
        patchcore/modules/PanCrossFadeModule.cpp
        patchcore/modules/PassModule.cpp
        patchcore/modules/SampleAndHoldModule.cpp
        patchcore/modules/VCAModule.cpp

        patchcore/modules/input/AbstractKeyboard.cpp
        patchcore/modules/input/cvGate/CvGateKeyboard.cpp

        patchcore/modules/input/event/keyboard/KeyboardModule.cpp
        patchcore/modules/input/event/keyboard/SingleKeyboardModule.cpp
        patchcore/modules/input/event/keyboard/poly/PolyProxyKeyboardModule.cpp
        patchcore/modules/input/event/keyboard/poly/AbstractRouterKeyboard.cpp
        patchcore/modules/input/event/keyboard/poly/RouterKeyboard.cpp

        patchcore/dsp/wavetable/DefaultWaveTableProvider.cpp
        patchcore/dsp/wavetable/mipmap/MipmapWaveTable.cpp
        patchcore/dsp/wavetable/mipmap/SawWaveTable.cpp
        patchcore/dsp/wavetable/mipmap/TriangleWaveTable.cpp
        patchcore/dsp/wavetable/mipmap/SimpleSquareWaveTable.cpp
        patchcore/dsp/wavetable/mipmap/SquareWaveTable.cpp
        patchcore/dsp/osc/NativeWaveOsc.cpp
        patchcore/dsp/osc/Noise.cpp
        patchcore/dsp/osc/BandLimitedSaw.cpp
        patchcore/dsp/osc/BandLimitedSquare.cpp
        patchcore/dsp/osc/BandLimitedTriangle.cpp
        patchcore/dsp/osc/BandLimitedSimpleSquare.cpp
        patchcore/dsp/waveshaper/SimpleWaveShaper.cpp

        patchcore/synth/BufferedSynth.cpp
        patchcore/synth/ModularSynth.cpp
        patchcore/test/ModularSynthTester.cpp

        utils/timeUtils.cpp
)

target_include_directories(PatchCore PUBLIC include)
target_include_directories(PatchCore PRIVATE private_include)

find_library(log-lib log)
target_link_libraries(PatchCore ${log-lib})

target_compile_options(PatchCore PRIVATE -Wall -Werror -Ofast -fno-finite-math-only)