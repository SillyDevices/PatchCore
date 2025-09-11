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


#ifndef PATCHCORE_AUDIOSTREAMCALLBACKPROXY_HPP
#define PATCHCORE_AUDIOSTREAMCALLBACKPROXY_HPP

#include <oboe/AudioStream.h>
#include <oboe/AudioStreamCallback.h>
#include <atomic>

class AudioStreamCallbackProxy: public oboe::AudioStreamCallback {

public:
    AudioStreamCallbackProxy(oboe::AudioStreamCallback* callback);
    virtual ~AudioStreamCallbackProxy() = default;

     oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) override;

    virtual bool onError(oboe::AudioStream*  audioStream,oboe:: Result error) override;
    virtual void onErrorBeforeClose(oboe::AudioStream* audioStream, oboe::Result error ) override;
    virtual void onErrorAfterClose(oboe::AudioStream* audioStream, oboe::Result error ) override;


    double getCpuLoad();
    double getCurrentLoad();

private:
    int64_t getNanoseconds(clockid_t clockId = CLOCK_MONOTONIC);

private:
    oboe::AudioStreamCallback* callback = nullptr;

    std::atomic<double>        mCpuLoad{0};
    std::atomic<double>        mCurrentLoad{0};
};

#endif //PATCHCORE_AUDIOSTREAMCALLBACKPROXY_HPP
