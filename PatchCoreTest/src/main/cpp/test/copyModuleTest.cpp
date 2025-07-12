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

#include <patchcore/module/factory/DefaultModuleFactory.hpp>
#include <gtest/gtest.h>
#include <patchcore/modules/ConstModule.hpp>

TEST(CopyModuleTest, constModuleCopyTest) {
    auto factory = new DefaultModuleFactory(44100);
    ASSERT_NE(factory, nullptr);

    auto constModule = factory->createModule(CONST_MODULE_TYPE_NAME, "testConst", 44100, {{CONST_MODULE_PARAMETER_VALUE, ModuleParameter(0.5f) } } );

    constModule->onStartBuffer(10);
    for (const auto &input: constModule->getInterpolatedInputs()) {
        input->onStartBuffer(10);
    }
    constModule->envelope();
    auto result = constModule->getModuleOutput(CONST_MODULE_OUTPUT)->value;
    ASSERT_FLOAT_EQ(result, 0.5f);

    //TODO copy and test
    auto copy = new ConstModule(*dynamic_cast<ConstModule*>(constModule.get()));

    copy->onStartBuffer(10);
    for (const auto &input: copy->getInterpolatedInputs()) {
        input->onStartBuffer(10);
    }
    copy->envelope();
    auto resultOfCopy = copy->getModuleOutput(CONST_MODULE_OUTPUT)->value;
    ASSERT_FLOAT_EQ(resultOfCopy, 0.5f);

    auto clone = constModule.get()->clone();
    ASSERT_NE(clone, nullptr);
    clone->onStartBuffer(10);
    for (const auto &input: clone->getInterpolatedInputs()) {
        clone->onStartBuffer(10);
    }
    clone->envelope();
    auto resultOfClone = clone->getModuleOutput(CONST_MODULE_OUTPUT)->value;
    ASSERT_FLOAT_EQ(resultOfClone, 0.5f);

    delete factory;
}