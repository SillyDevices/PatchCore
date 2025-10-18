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


#ifndef PATCHCORE_OBOEAUDIOINTERFACE_HPP
#define PATCHCORE_OBOEAUDIOINTERFACE_HPP

#include <mutex>
#include <oboe/Oboe.h>
#include <patchcore/synth/ModularSynth.hpp>
#include <oboe/StabilizedCallback.h>
#include "utils/CpuLoadCounter.hpp"
#include "utils/AudioStreamCallbackProxy.hpp"
#include <vector>


#define WAIT_TIME 10000
#define XRUN_LIMIT 100
#define AFFINITY_FRAMES 3*44100
#define DEBUG false

class OboeAudioInterface: public oboe::AudioStreamCallback {
public:
    struct PlayerOptions {
        int bufferSizeMultiplayer;
        bool useStabilizedCallback;
        bool useCpuAffinity;
        bool useGameMode;
        bool useBestCpuByMaxId;
        std::vector<int> preferredCpuIds;
    };

    struct DebugInfo {
        int xRunCount;
        int cpuId;
        int bufferSizeInFrames;
        double cpuLoad;
    };



public:
    OboeAudioInterface();
    virtual ~OboeAudioInterface() = default;
    int32_t startAudio();
    void stopAudio();

    bool isStarted();

    void setOptions(PlayerOptions options);

public:
    void setSynth(ModularSynth *synth);

private:
    void setBufferSizeMultiplayer(int mult);

public:
    DebugInfo getDebugInfo();

private:
    int getXRunCount();
    int getCpuId();
    int getBufferSizeInFrames();
    double getCpuLoad();

public:
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result result) override;

private:
    bool setThreadAffinity(int preferredCpuId);

private:
    PlayerOptions options;

    std::mutex mLock;
    std::shared_ptr<oboe::AudioStream> mStream;
    ModularSynth *mSynth = nullptr;

    std::unique_ptr<AudioStreamCallbackProxy> proxy;
    std::unique_ptr<oboe::StabilizedCallback> stabilized;

    CpuLoadCounter cpuLoadCounter = CpuLoadCounter();

    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 44100;

private:

    bool isAffinitySet = false;
    int waitAffinityFramesCounter = 0;




    int lastCpuId = -1;
    int currentCpuId = -1;
    int preferredCpuId = -1;

    bool mIsStarted = false;
};

#endif //PATCHCORE_OBOEAUDIOINTERFACE_HPP
