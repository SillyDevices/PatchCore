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
#import "DefaultModuleFactoryWrapper.h"
#include <patchcore/module/factory/ModuleFactory.hpp>
#include <patchcore/module/factory/DefaultModuleFactory.hpp>


@implementation DefaultModuleFactoryWrapper {
    ModuleFactory* _customModuleFactory;
}

- (instancetype)initWithWaveTableProvider:(uintptr_t)waveTableProvider :(uintptr_t)customModuleFactory {
    WaveTableProvider *provider = reinterpret_cast<WaveTableProvider*>(waveTableProvider);
    ModuleFactory* customFactory = reinterpret_cast<ModuleFactory*>(customModuleFactory);
    
    ModuleFactory* factory = new DefaultModuleFactory(provider, customFactory);
    self = [super initWithModuleFactory:factory];
    if (self) {
        _customModuleFactory = factory;
    } else {
        delete factory;
        return nil;
    }
    return self;
}

- (void)dealloc {
    //TODO should wrap _customModuleFactory to some managed object
    //good for now, i can't imagine a use case with more than one DefaultModuleFactory
    delete _customModuleFactory;
}


@end
