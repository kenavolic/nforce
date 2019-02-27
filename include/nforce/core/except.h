// Copyright 2019 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <exception>
#include <string>
#include <type_traits>

#include "status.h"

namespace n4 {
///
/// @brief Internal exception class
///
class nexcept : virtual public std::exception {
public:
  nexcept(const std::string &str, status_type s);
  virtual status_type status() const noexcept;

private:
  status_type m_status;
};

///
/// @brief Exception conversion utilities
///
template <typename Callable,
          typename = std::enable_if_t<std::is_invocable_v<Callable>>>
status_type translate(Callable &&f) {
  try {
    std::forward<Callable>(f)();
    return status_type::SUCCESS;
  } catch (const nexcept &ex) {
    return ex.status();
  } catch (...) {
    return status_type::UNKNOWN_ERROR;
  }
}
} // namespace n4