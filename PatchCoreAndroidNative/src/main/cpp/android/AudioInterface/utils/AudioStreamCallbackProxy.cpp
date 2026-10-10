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


#include "AudioStreamCallbackProxy.hpp"

AudioStreamCallbackProxy::AudioStreamCallbackProxy(oboe::AudioStreamCallback *callback): callback(callback) {

}

oboe::DataCallbackResult
AudioStreamCallbackProxy::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    int64_t startTimeNanos = getNanoseconds();

    auto result = callback->onAudioReady(audioStream, audioData, numFrames);

    // Update CPU load
    double calculationTime = (double)(getNanoseconds() - startTimeNanos);
    double inverseRealTime = audioStream->getSampleRate() / (1.0e9 * numFrames);
    double currentCpuLoad = calculationTime * inverseRealTime; // avoid a divide
    mCpuLoad = (mCpuLoad * 0.97) + (currentCpuLoad * 0.03); // simple low pass filter
    mCurrentLoad = currentCpuLoad;

    return result;
}

int64_t AudioStreamCallbackProxy::getNanoseconds(clockid_t clockId) {
    struct timespec time;
    int result = clock_gettime(clockId, &time);
    if (result < 0) {
        return result;
    }
    return (time.tv_sec * 1e9) + time.tv_nsec;
}

double AudioStreamCallbackProxy::getCpuLoad() {
    return mCpuLoad;
}

bool AudioStreamCallbackProxy::onError(oboe::AudioStream *audioStream, oboe::Result error) {
    return callback->onError(audioStream, error);
}

void
AudioStreamCallbackProxy::onErrorBeforeClose(oboe::AudioStream *audioStream, oboe::Result error) {
    callback->onErrorBeforeClose(audioStream, error);
}

void
AudioStreamCallbackProxy::onErrorAfterClose(oboe::AudioStream *audioStream, oboe::Result error) {
    callback->onErrorAfterClose(audioStream, error);
}

double AudioStreamCallbackProxy::getCurrentLoad() {
    return mCurrentLoad;
}




