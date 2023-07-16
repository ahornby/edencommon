# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(os_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/ProcessId.h
)

set(os_srcs
  ${CMAKE_CURRENT_SOURCE_DIR}/ProcessId.cpp
)

add_library(edencommon_os ${os_headers} ${os_srcs})

target_link_libraries(edencommon_os
    Folly::folly
    glog::glog
)

target_include_directories(edencommon_os PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

install(
  TARGETS edencommon_os
  EXPORT edencommon-exports
  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
)

install(
  FILES ${utils_headers}
  DESTINATION ${INCLUDE_INSTALL_DIR}/eden/common/os
)

add_subdirectory(test)
