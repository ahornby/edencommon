/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/utils/Memory.h"

#include <folly/Exception.h>
#include <folly/portability/GTest.h>

using std::string;
using namespace facebook::eden;

TEST(Memory, StdStringMemoryUsage) {
  std::string test;
  for (int stringLength = 0; stringLength < 100; stringLength++) {
    SCOPED_TRACE(folly::to<string>("string length = ", stringLength));
    std::byte* data = reinterpret_cast<std::byte*>(test.data());
    std::byte* stringloc = reinterpret_cast<std::byte*>(&test);
    bool isSmall =
        data >= stringloc && data < (stringloc + sizeof(std::string));
    size_t allocated = isSmall ? 0 : test.capacity();
    ASSERT_EQ(
        folly::goodMallocSize(allocated), estimateIndirectMemoryUsage(test));
    test += "p";
  }
}
#if !FOLLY_LIBRARY_SANITIZE_ADDRESS
/*
 * Test case disabled when Address Sanitation is on since SSO optimization is
 * disabled when Address Sanitation is enabled
 */
TEST(Memory, FBStringMemoryUsage) {
  folly::fbstring test("");
  for (int i = 0; i < 100; i++) {
    char* data = test.data();
    char* stringloc = (char*)(&test);
    bool isSmall =
        data >= stringloc && data < (stringloc + sizeof(folly::fbstring));
    size_t allocated = isSmall ? 0 : test.capacity();
    ASSERT_EQ(
        folly::goodMallocSize(allocated), estimateIndirectMemoryUsage(test));
    test += "p";
  }
}
#endif
