/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file implements unit tests for FabricTable implementation.
 */

#include <errno.h>
#include <nlunit-test.h>

#include <lib/core/CHIPCore.h>

#include <credentials/FabricTable.h>

#include <lib/support/CodeUtils.h>
#include <lib/support/UnitTestRegistration.h>
#include <stdarg.h>

using namespace chip;
using namespace Transport;

static const uint8_t sTestRootCert[] = {
    0x15, 0x30, 0x01, 0x08, 0x59, 0xea, 0xa6, 0x32, 0x94, 0x7f, 0x54, 0x1c, 0x24, 0x02, 0x01, 0x37, 0x03, 0x27, 0x14, 0x01, 0x00,
    0x00, 0x00, 0xca, 0xca, 0xca, 0xca, 0x18, 0x26, 0x04, 0xef, 0x17, 0x1b, 0x27, 0x26, 0x05, 0x6e, 0xb5, 0xb9, 0x4c, 0x37, 0x06,
    0x27, 0x14, 0x01, 0x00, 0x00, 0x00, 0xca, 0xca, 0xca, 0xca, 0x18, 0x24, 0x07, 0x01, 0x24, 0x08, 0x01, 0x30, 0x09, 0x41, 0x04,
    0x13, 0x53, 0xa3, 0xb3, 0xef, 0x1d, 0xa7, 0x08, 0xc4, 0x90, 0x80, 0x48, 0x01, 0x4e, 0x40, 0x7d, 0x59, 0x90, 0xce, 0x22, 0xbc,
    0x4e, 0xb3, 0x3e, 0x9a, 0x5a, 0xcb, 0x25, 0xa8, 0x56, 0x03, 0xeb, 0xa6, 0xdc, 0xd8, 0x21, 0x36, 0x66, 0xa4, 0xe4, 0x4f, 0x5a,
    0xca, 0x13, 0xeb, 0x76, 0x7f, 0xaf, 0xa7, 0xdc, 0xdd, 0xdc, 0x33, 0x41, 0x1f, 0x82, 0xa3, 0x0b, 0x54, 0x3d, 0xd1, 0xd2, 0x4b,
    0xa8, 0x37, 0x0a, 0x35, 0x01, 0x29, 0x01, 0x18, 0x24, 0x02, 0x60, 0x30, 0x04, 0x14, 0x13, 0xaf, 0x81, 0xab, 0x37, 0x37, 0x4b,
    0x2e, 0xd2, 0xa9, 0x64, 0x9b, 0x12, 0xb7, 0xa3, 0xa4, 0x28, 0x7e, 0x15, 0x1d, 0x30, 0x05, 0x14, 0x13, 0xaf, 0x81, 0xab, 0x37,
    0x37, 0x4b, 0x2e, 0xd2, 0xa9, 0x64, 0x9b, 0x12, 0xb7, 0xa3, 0xa4, 0x28, 0x7e, 0x15, 0x1d, 0x18, 0x30, 0x0b, 0x40, 0x45, 0x81,
    0x64, 0x46, 0x6c, 0x8f, 0x19, 0x5a, 0xbc, 0x0a, 0xbb, 0x7c, 0x6c, 0xb5, 0xa2, 0x7a, 0x83, 0xf4, 0x1d, 0x37, 0xf8, 0xd5, 0x3b,
    0xee, 0xc5, 0x20, 0xab, 0xd2, 0xa0, 0xda, 0x05, 0x09, 0xb8, 0xa7, 0xc2, 0x5c, 0x04, 0x2e, 0x30, 0xcf, 0x64, 0xdc, 0x30, 0xfe,
    0x33, 0x4e, 0x12, 0x00, 0x19, 0x66, 0x4e, 0x51, 0x50, 0x49, 0x13, 0x4f, 0x57, 0x81, 0x23, 0x84, 0x44, 0xfc, 0x75, 0x31, 0x18,
};

void TestGetCompressedFabricID(nlTestSuite * inSuite, void * inContext)
{
    FabricInfo fabricInfo;

    NL_TEST_ASSERT(inSuite, fabricInfo.SetRootCert(ByteSpan(sTestRootCert)) == CHIP_NO_ERROR);

    PeerId compressedId;
    NL_TEST_ASSERT(inSuite, fabricInfo.GetCompressedId(1234, 4321, &compressedId) == CHIP_NO_ERROR);

    // We are compairing with hard coded values here (which are generated manually when the test was written)
    // This is to ensure that the same value is generated on big endian and little endian platforms.
    // If in this test any input to GetCompressedId() is changed, this value must be recomputed.
    NL_TEST_ASSERT(inSuite, compressedId.GetCompressedFabricId() == 0x090F17C67be7b663);
    NL_TEST_ASSERT(inSuite, compressedId.GetNodeId() == 4321);

    NL_TEST_ASSERT(inSuite, fabricInfo.GetCompressedId(0xabcd, 0xdeed, &compressedId) == CHIP_NO_ERROR);

    // We are compairing with hard coded values here (which are generated manually when the test was written)
    // This is to ensure that the same value is generated on big endian and little endian platforms
    // If in this test any input to GetCompressedId() is changed, this value must be recomputed.
    NL_TEST_ASSERT(inSuite, compressedId.GetCompressedFabricId() == 0xf3fecbcec485d5d7);
    NL_TEST_ASSERT(inSuite, compressedId.GetNodeId() == 0xdeed);
}

// Test Suite

/**
 *  Test Suite that lists all the test functions.
 */
// clang-format off
static const nlTest sTests[] =
{
    NL_TEST_DEF("Compressed Fabric ID",    TestGetCompressedFabricID),
    NL_TEST_SENTINEL()
};
// clang-format on

int TestFabricTable_Setup(void * inContext);
int TestFabricTable_Teardown(void * inContext);

// clang-format off
static nlTestSuite sSuite =
{
    "Test-CHIP-FabricTable",
    &sTests[0],
    TestFabricTable_Setup,
    TestFabricTable_Teardown,
};
// clang-format on

/**
 *  Set up the test suite.
 */
int TestFabricTable_Setup(void * inContext)
{
    return chip::Platform::MemoryInit() == CHIP_NO_ERROR ? SUCCESS : FAILURE;
}

/**
 *  Tear down the test suite.
 */
int TestFabricTable_Teardown(void * inContext)
{
    chip::Platform::MemoryShutdown();
    return SUCCESS;
}

/**
 *  Main
 */
int TestFabricTable()
{
    // Run test suit against one context
    nlTestRunner(&sSuite, nullptr);

    return (nlTestRunnerStats(&sSuite));
}

CHIP_REGISTER_TEST_SUITE(TestFabricTable)