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
#import "WaveTableProviderWrapper.h"
#include <patchcore/dsp/wavetable/WaveTableProvider.hpp>


@implementation WaveTableProviderWrapper {
    WaveTableProvider *_provider;
}

- (instancetype)initWithProvider:(void*)provider {
    self = [super init];
    if (self) {
        _provider = reinterpret_cast<WaveTableProvider *>(provider);
    }
    return self;
}

- (void)dealloc {
    delete _provider;
}

- (uintptr_t)getRawPointerToWaveTableProvider {
    return reinterpret_cast<uintptr_t>(_provider);
}

@end
