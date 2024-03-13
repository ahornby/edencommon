/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/telemetry/StatsGroup.h"

#include <chrono>

#include <folly/logging/xlog.h>

namespace facebook::eden {

StatsGroupBase::Counter::Counter(std::string_view name)
    : Stat{
          name,
          fb303::ExportTypeConsts::kSumCountAvgRate,
          // Don't record quantiles for counters. Usually "1" is the only value
          // added. Usually we care about counts and rates.
          {},
          fb303::SlidingWindowPeriodConsts::kOneMinTenMinHour,
      } {
  // TODO: enforce the name matches the StatsGroup prefix.
}

StatsGroupBase::Duration::Duration(std::string_view name)
    : Stat{
          name,
          fb303::ExportTypeConsts::kSumCountAvgRate,
          fb303::QuantileConsts::kP1_P10_P50_P90_P99,
          fb303::SlidingWindowPeriodConsts::kOneMinTenMinHour} {
  // This should be a compile-time check but I don't know how to spell that in a
  // convenient way. :) Asserting at startup in debug mode should be sufficient.
  XCHECK_GT(name.size(), size_t{3}) << "duration name too short";
  XCHECK_EQ("_us", std::string_view(name.data() + name.size() - 3, 3))
      << "duration stats must end in _us";
  // TODO: enforce the name matches the StatsGroup prefix.
}

void StatsGroupBase::Duration::addDuration(std::chrono::microseconds elapsed) {
  addValue(elapsed.count());
}

} // namespace facebook::eden
