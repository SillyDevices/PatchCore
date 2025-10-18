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


#include "OboeAudioInterface.hpp"
#include <android/log_macros.h>
#include <stdexcept>
#include <string>
#include <oboe/StabilizedCallback.h>
#include <thread>
#include "unistd.h"

#define MODULE_NAME "PLAYER"

OboeAudioInterface::OboeAudioInterface(){
    proxy = std::make_unique<AudioStreamCallbackProxy>(this);
}

void OboeAudioInterface::setOptions(OboeAudioInterface::PlayerOptions options) {
    PlayerOptions oldOptions = this->options;
    PlayerOptions newOptions = {
            .bufferSizeMultiplayer = options.bufferSizeMultiplayer,
            .useStabilizedCallback = options.useStabilizedCallback,
            .useCpuAffinity = options.useCpuAffinity,
            .useGameMode = options.useGameMode,
            .useBestCpuByMaxId = options.useBestCpuByMaxId,
            .preferredCpuIds = options.preferredCpuIds
    };
    this->options = newOptions;
    if (oldOptions.bufferSizeMultiplayer != newOptions.bufferSizeMultiplayer) {
        setBufferSizeMultiplayer(newOptions.bufferSizeMultiplayer);
    }
}

void OboeAudioInterface::setSynth(ModularSynth *synth) {
    mSynth = synth;
}

void OboeAudioInterface::setBufferSizeMultiplayer(int multiplayer) {
    if (mStream) {
        auto framesPerBurst = mStream->getFramesPerBurst();
        auto capacity = mStream->getBufferCapacityInFrames();
        if (framesPerBurst > 0){
            if (framesPerBurst <= capacity) {
                mStream->setBufferSizeInFrames(framesPerBurst * multiplayer);
            } else {
                mStream->setBufferSizeInFrames(capacity);
            }
        }
    }
}

int32_t OboeAudioInterface::startAudio() {
    std::lock_guard<std::mutex> lock(mLock);

    waitAffinityFramesCounter = 0;
    isAffinitySet = false;
    preferredCpuId = -1;
    oboe::Usage usage;
    if (options.useGameMode) {
        usage = oboe::Usage::Game;
    } else {
        usage = oboe::Usage::Media;
    }

    oboe::AudioStreamBuilder builder;
    builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setChannelCount(kChannelCount)
            ->setSampleRate(kSampleRate)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Fastest)
            ->setContentType(oboe::ContentType::Music)
            ->setUsage(usage)
            ->setFormat(oboe::AudioFormat::Float);
    if (options.useStabilizedCallback){
        stabilized = std::make_unique<oboe::StabilizedCallback>(proxy.get());
        builder.setDataCallback(stabilized.get());
        builder.setErrorCallback(stabilized.get());
    } else {
        builder.setDataCallback(proxy.get());
        builder.setErrorCallback(proxy.get());
    }
    oboe::Result result = builder.openStream(mStream);
    if (result != oboe::Result::OK) {
        return (int32_t) result;
    }

    result = mStream->requestStart();

    setBufferSizeMultiplayer(options.bufferSizeMultiplayer);

    mIsStarted = true;
    return (int32_t) result;
}

void OboeAudioInterface::stopAudio() {
    std::lock_guard<std::mutex> lock(mLock);
    if (mStream) {
        mStream->stop();
        mStream->close();
        mStream.reset();
        mStream = nullptr;
    }
    mIsStarted = false;
}

bool OboeAudioInterface::isStarted() {
    return mIsStarted;
}

OboeAudioInterface::DebugInfo OboeAudioInterface::getDebugInfo() {
    return {
        .xRunCount = getXRunCount(),
        .cpuId = getCpuId(),
        .bufferSizeInFrames = getBufferSizeInFrames(),
        .cpuLoad = getCpuLoad()
    };
}

int OboeAudioInterface::getXRunCount() {
    if (mStream) {
        auto result = mStream->getXRunCount();
        if (result.operator bool()) {
            return result.value();
        }
    }
    return 0;
}

int OboeAudioInterface::getBufferSizeInFrames() {
    if (mStream) {
        return mStream->getBufferSizeInFrames();
    }
    return 0;
}

int OboeAudioInterface::getCpuId() {
    return currentCpuId;
}

double OboeAudioInterface::getCpuLoad() {
    AudioStreamCallbackProxy* ptr = proxy.get();
    if (ptr != nullptr) {
        return ptr->getCpuLoad();
    }
    return 0.0;
}

oboe::DataCallbackResult OboeAudioInterface::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    currentCpuId = sched_getcpu();

    if (!isAffinitySet) {
        if (options.useBestCpuByMaxId) {
            preferredCpuId = std::thread::hardware_concurrency() - 1;
            setThreadAffinity(preferredCpuId);
            isAffinitySet = true;
        } else {
            if (!options.useCpuAffinity) {
                isAffinitySet = true;
            } else {
                if (waitAffinityFramesCounter < AFFINITY_FRAMES) {
                    auto lastLoad = proxy.get()->getCurrentLoad();
                    if (lastCpuId != -1) {
                        cpuLoadCounter.updateCpuLoad(lastCpuId, lastLoad);
                    }
                    lastCpuId = currentCpuId;
                    waitAffinityFramesCounter += numFrames;
                } else {
                    if (preferredCpuId == -1) {
                        preferredCpuId = cpuLoadCounter.getBestCpuId();
                    }
                    isAffinitySet = setThreadAffinity(preferredCpuId);
                }
            }
        }
    } else {
        if (preferredCpuId != currentCpuId && preferredCpuId != -1 && options.useCpuAffinity) {
            setThreadAffinity(preferredCpuId);
        }
    }

    auto floatData = (float *) audioData;
    if (mSynth != nullptr) {
        mSynth->onStartBuffer(numFrames);
        for (auto i = 0; i < numFrames; ++i) {
            auto result = mSynth->computeSample();
            floatData[i* kChannelCount + 0] = result.first;
            floatData[i* kChannelCount + 1] = result.second;
        }
//        mSynth->onEndBuffer();
    }

    return oboe::DataCallbackResult::Continue;
}



void OboeAudioInterface::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result result) {
    __android_log_print(ANDROID_LOG_ERROR, "ENGINE", "onErrorAfterClose %d", result);
    stopAudio();
    startAudio();
}

bool OboeAudioInterface::setThreadAffinity(int preferredCpuId = -1) {
    if (options.preferredCpuIds.size() > 0){
        pid_t current_thread_id = gettid();
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        for (size_t i = 0; i < options.preferredCpuIds.size(); i++) {
            int cpu_id = options.preferredCpuIds.at(i);
            CPU_SET(cpu_id, &cpu_set);
        }
        int result = sched_setaffinity(current_thread_id, sizeof(cpu_set_t), &cpu_set);
        return true;
    } else {
        pid_t current_thread_id = gettid();
        if (preferredCpuId != -1) {
            cpu_set_t cpu_set;
            CPU_ZERO(&cpu_set);
            CPU_SET(preferredCpuId, &cpu_set);
            int result = sched_setaffinity(current_thread_id, sizeof(cpu_set_t), &cpu_set);
            if (result == 0) {
                return true;
            }
        }
        {
            int current_cpu_id = sched_getcpu();
            cpu_set_t cpu_set;
            CPU_ZERO(&cpu_set);
            CPU_SET(current_cpu_id, &cpu_set);

            int result = sched_setaffinity(current_thread_id, sizeof(cpu_set_t), &cpu_set);
            if (result == 0) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }
    return false;
}