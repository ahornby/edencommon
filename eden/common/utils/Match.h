/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <variant>

namespace facebook::eden {

namespace detail {

template <typename... Cases>
struct overloaded : Cases... {
  overloaded(Cases&&... cases) : Cases{std::forward<Cases>(cases)}... {}

  using Cases::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace detail

/**
 * Type-safe, ergonomic pattern matching on std::variant.
 *
 * See MatchTest.cpp for examples.
 */
template <typename Variant, typename... Cases>
decltype(auto) match(Variant&& v, Cases&&... cases) {
  return std::visit(
      detail::overloaded(std::forward<Cases>(cases)...),
      std::forward<Variant>(v));
}

} // namespace facebook::eden
