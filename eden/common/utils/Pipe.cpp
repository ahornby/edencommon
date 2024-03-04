/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/utils/Pipe.h"

#include <fcntl.h>
#include <folly/Exception.h>
#include <folly/String.h>
#ifndef _WIN32
#include <folly/portability/Sockets.h>
#include <folly/portability/Unistd.h>
#else
#include <event2/util.h> // @manual
#endif
#include <system_error>

namespace facebook::eden {

Pipe::Pipe(bool nonBlocking) {
#ifdef _WIN32
  (void)nonBlocking;
  HANDLE readPipe;
  HANDLE writePipe;
  auto sec = SECURITY_ATTRIBUTES{};

  sec.nLength = sizeof(sec);
  sec.bInheritHandle = FALSE; // O_CLOEXEC equivalent
  constexpr DWORD kPipeSize = 64 * 1024;

  if (!CreatePipe(&readPipe, &writePipe, &sec, kPipeSize)) {
    throw std::system_error(
        GetLastError(), std::system_category(), "CreatePipe failed");
  }
  read = FileDescriptor(
      reinterpret_cast<intptr_t>(readPipe), FileDescriptor::FDType::Pipe);
  write = FileDescriptor(
      reinterpret_cast<intptr_t>(writePipe), FileDescriptor::FDType::Pipe);

#else
  int fds[2];
  int res;
#if FOLLY_HAVE_PIPE2
  res = pipe2(fds, (nonBlocking ? O_NONBLOCK : 0) | O_CLOEXEC);
#else
  res = pipe(fds);
#endif

  if (res) {
    throw std::system_error(
        errno,
        std::system_category(),
        std::string("pipe error: ") + folly::errnoStr(errno));
  }
  read = FileDescriptor(fds[0], FileDescriptor::FDType::Pipe);
  write = FileDescriptor(fds[1], FileDescriptor::FDType::Pipe);

#if !FOLLY_HAVE_PIPE2
  read.setCloExec();
  write.setCloExec();
  if (nonBlocking) {
    read.setNonBlock();
    write.setNonBlock();
  }
#endif
#endif
}

SocketPair::SocketPair(bool nonBlocking) {
  FileDescriptor::system_handle_type pair[2];
#ifdef _WIN32
  // The win32 libevent implementation will attempt to use unix domain sockets
  // if available, but will fall back to using loopback TCP sockets.
  auto r = evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
#else // !_WIN32
  auto r = ::socketpair(
      AF_UNIX,
#ifdef SOCK_NONBLOCK
      SOCK_NONBLOCK |
#endif // SOCK_NONBLOCK
#ifdef SOCK_CLOEXEC
          SOCK_CLOEXEC |
#endif // SOCK_CLOEXEC
          SOCK_STREAM,
      0,
      pair);
#endif // !_WIN32
  folly::checkUnixError(r, "socketpair failed");

  read = FileDescriptor(pair[0], FileDescriptor::FDType::Socket);
  write = FileDescriptor(pair[1], FileDescriptor::FDType::Socket);

  read.setCloExec();
  write.setCloExec();
  if (nonBlocking) {
    read.setNonBlock();
    write.setNonBlock();
  }
}

} // namespace facebook::eden
