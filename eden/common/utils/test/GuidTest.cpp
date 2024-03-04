/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifdef _WIN32

#include "eden/common/utils/Guid.h"
#include <fmt/format.h>
#include <folly/portability/GTest.h>
#include <iostream>
#include <string>

using namespace facebook::eden;

TEST(GuidTest, assignedGuid) {
  // {811305DA-F51E-4E2D-9201-0D12A1E7F8D5}
  static const GUID testGuid = {
      0x811305da,
      0xf51e,
      0x4e2d,
      {0x92, 0x1, 0xd, 0x12, 0xa1, 0xe7, 0xf8, 0xd5}};

  Guid guid{testGuid};

  EXPECT_EQ(guid.getGuid(), testGuid);
}

TEST(GuidTest, emptyGuid) {
  static const GUID testGuid{0};
  Guid guid;

  EXPECT_EQ(guid.getGuid(), testGuid);
}

TEST(GuidTest, generatedGuid) {
  Guid guid;
  // Use Assignment operator
  guid = Guid::generate();
  Guid testGuid{guid};

  EXPECT_EQ(guid.getGuid(), testGuid.getGuid());
}

TEST(GuidTest, compareGuids) {
  // {811305DA-F51E-4E2D-9201-0D12A1E7F8D5}
  static const GUID testGuid = {
      0x811305da,
      0xf51e,
      0x4e2d,
      {0x92, 0x1, 0xd, 0x12, 0xa1, 0xe7, 0xf8, 0xd5}};

  Guid guid1{testGuid};
  Guid guid2;
  Guid guid3;
  Guid guid4{Guid::generate()};

  guid2 = testGuid;

  EXPECT_EQ(guid1, guid2);
  EXPECT_NE(guid1, guid3);
  EXPECT_NE(guid1, guid4);
}

TEST(GuidTest, pointerGuids) {
  // {811305DA-F51E-4E2D-9201-0D12A1E7F8D5}
  static const GUID testGuid = {
      0x811305da,
      0xf51e,
      0x4e2d,
      {0x92, 0x1, 0xd, 0x12, 0xa1, 0xe7, 0xf8, 0xd5}};

  Guid guid1{testGuid};
  const GUID* ptrGuid1 = guid1;
  const GUID* ptrGuid2 = &testGuid;
  Guid guid2;
  Guid guid3{*ptrGuid2};

  guid2 = *ptrGuid1;

  EXPECT_EQ(guid1, guid2);
  EXPECT_EQ(guid1, guid3);
}

TEST(GuidTest, formatGuid) {
  static const GUID testGuid = {
      0x811305da,
      0xf51e,
      0x4e2d,
      {0x92, 0x1, 0xd, 0x12, 0xa1, 0xe7, 0xf8, 0xd5}};
  Guid guid1{testGuid};
  auto s = fmt::format(FMT_STRING("{}"), guid1);
  EXPECT_EQ(s, "{811305DA-F51E-4E2D-9201-0D12A1E7F8D5}");
}

TEST(GuidTest, fromString) {
  Guid guid{"811305DA-F51E-4E2D-9201-0D12A1E7F8D5"};
  auto s = fmt::format(FMT_STRING("{}"), guid);
  EXPECT_EQ(s, "{811305DA-F51E-4E2D-9201-0D12A1E7F8D5}");
}

#endif
