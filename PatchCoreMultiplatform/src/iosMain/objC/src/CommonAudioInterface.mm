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


#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import "CommonAudioInterface.h"
#import <patchcore/module/buffer/BlockConfig.hpp>
#import <patchcore/synth/ModularSynth.hpp>
#import <patchcore/module/PatchModule.hpp>

#include <algorithm>


@implementation CommonAudioInterface {
    bool _started;
    
    AVAudioEngine* _audioEngine;
    AVAudioMixerNode *_mainMixer;
    AVAudioOutputNode *_output;
    AVAudioFormat *_outputFormat;
    AVAudioFormat *_inputFormat;
    AVAudioSourceNode *_srcNode;
    float _sampleRate;
    
    ModularSynth* _modularSynth;
    StereoBlock _pendingBlock;
    int _pendingBlockIndex;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _started = FALSE;
        _audioEngine = [[AVAudioEngine alloc] init];
        _mainMixer = _audioEngine.mainMixerNode;
        _output = _audioEngine.outputNode;
        _outputFormat = [_output inputFormatForBus:0];
        int outputSampleRate = (float)_outputFormat.sampleRate;
        if (_sampleRate != outputSampleRate) {
            NSLog(@"SampleRates don't match");
        }
        
        _inputFormat = [[AVAudioFormat alloc] initWithCommonFormat:_outputFormat.commonFormat
                                                         sampleRate:_outputFormat.sampleRate
                                                           channels:2
                                                        interleaved:_outputFormat.isInterleaved];
        
        [_audioEngine connect:_mainMixer to:_output format:_outputFormat];
        _mainMixer.outputVolume = 0.5;
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(engineConfigurationChange:)
                                             name:AVAudioEngineConfigurationChangeNotification
                                             object:_audioEngine];
        _modularSynth = nullptr;
        _pendingBlockIndex = PATCHCORE_BLOCK_SIZE;
        
    }
    return self;
}

- (void)engineConfigurationChange:(NSNotification *)notification {
    [self internalStop];
    [self internalStart];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void) setModularSynth:(uintptr_t) modularSynthPointer {
    auto synth = reinterpret_cast<PatchModule*>(modularSynthPointer);
    if (synth == nullptr) throw std::runtime_error("modularSynthPointer is not PatchModule");
    _modularSynth = dynamic_cast<ModularSynth*>(synth);
    if (_modularSynth == nullptr) throw std::runtime_error("modularSynthPointer is not ModularSynth");
    _pendingBlockIndex = PATCHCORE_BLOCK_SIZE;
}

- (bool)internalStart {
    __weak __typeof(self) weakSelf = self;
    _pendingBlockIndex = PATCHCORE_BLOCK_SIZE;
    _srcNode = [[AVAudioSourceNode alloc] initWithRenderBlock:
        ^OSStatus(BOOL *isSilence, const AudioTimeStamp *timestamp, AVAudioFrameCount frameCount, AudioBufferList *audioBufferList) {
            __strong __typeof(weakSelf) strongSelf = weakSelf;
            if (strongSelf) {
                AudioBufferList *ablPointer = audioBufferList;
                [strongSelf renderWithFrameCount:(int)frameCount audioBufferList:ablPointer];
            }
            return noErr;
        }
    ];
    
    if (_srcNode) {
        [_audioEngine attachNode:_srcNode];
        [_audioEngine connect:_srcNode to:_mainMixer format:_inputFormat];
    }
    
#if TARGET_OS_IPHONE
        AVAudioSession *session = [AVAudioSession sharedInstance];
        NSError *sessionError = nil;
        [session setCategory:AVAudioSessionCategoryPlayback error:&sessionError];
        [session setActive:YES error:&sessionError];
#endif
    
    NSError *error = nil;
    if (![_audioEngine startAndReturnError:&error]) {
        NSLog(@"Could not start engine: %@", error);
        return false;
    }
    return true;
}

- (void)internalStop {
    _pendingBlockIndex = PATCHCORE_BLOCK_SIZE;
    if (_srcNode) {
        [_audioEngine disconnectNodeInput:_mainMixer];
        [_audioEngine detachNode:_srcNode];
    }
}

- (void)renderWithFrameCount:(int)frameCount audioBufferList:(AudioBufferList *)audioBufferList {
    if (audioBufferList->mNumberBuffers < 2) {
        return;
    }
    float *left = (float *)audioBufferList->mBuffers[0].mData;
    float *right = (float *)audioBufferList->mBuffers[1].mData;

    if (_modularSynth != nullptr) {
        int frameIndex = 0;
        while (frameIndex < frameCount) {
            if (_pendingBlockIndex >= PATCHCORE_BLOCK_SIZE) {
                _modularSynth->onStartBuffer(PATCHCORE_BLOCK_SIZE);
                _modularSynth->computeBlock(_pendingBlock);
                _pendingBlockIndex = 0;
            }

            auto framesToCopy = std::min(frameCount - frameIndex, PATCHCORE_BLOCK_SIZE - _pendingBlockIndex);
            for (int offset = 0; offset < framesToCopy; ++offset) {
                auto sampleIndex = _pendingBlockIndex + offset;
                auto outputIndex = frameIndex + offset;
                left[outputIndex] = _pendingBlock.left[sampleIndex];
                right[outputIndex] = _pendingBlock.right[sampleIndex];
            }

            frameIndex += framesToCopy;
            _pendingBlockIndex += framesToCopy;
        }
    }
}

- (bool)isStarted {
    return _started;
}

- (bool)startAudio {
    _started = [self internalStart];
    return _started;
}

- (void)stopAudio {
    [self internalStop];
}

@end
