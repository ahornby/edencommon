/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/utils/UnboundedQueueExecutor.h"

#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/ManualExecutor.h>
#include <folly/executors/task_queue/UnboundedBlockingQueue.h>
#include <folly/executors/thread_factory/NamedThreadFactory.h>

namespace facebook::eden {

UnboundedQueueExecutor::UnboundedQueueExecutor(
    size_t threadCount,
    folly::StringPiece threadNamePrefix)
    : executor_{std::make_unique<folly::CPUThreadPoolExecutor>(
          threadCount,
          std::make_unique<folly::UnboundedBlockingQueue<
              folly::CPUThreadPoolExecutor::CPUTask>>(),
          std::make_unique<folly::NamedThreadFactory>(threadNamePrefix))} {}

UnboundedQueueExecutor::UnboundedQueueExecutor(
    std::shared_ptr<folly::ManualExecutor> executor)
    : executor_{std::move(executor)} {}

size_t UnboundedQueueExecutor::getTaskQueueSize() const {
  if (auto ex =
          std::dynamic_pointer_cast<folly::CPUThreadPoolExecutor>(executor_)) {
    return ex->getTaskQueueSize();
  }
  // manual executor does not expose a way to count the number of pending
  // tasks.
  return 0;
}

} // namespace facebook::eden
