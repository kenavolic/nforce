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

#include <optional>
#include <stack>
#include <string>

namespace n4 {
///
/// @brief Token type matching the grammar
///
enum class token_type { LEFT = 0, RIGHT, AND, OR, NOT, RULE, END };

using token = std::pair<token_type, std::optional<std::string>>;

///
/// @brief Perform on-the-fly lexical analysis
///        of an input boolean expression representing
///        a set of rules to enforce
///
/// Internal grammar matches a standard boolean grammar
/// except that terminal expressions are rules provided
/// within quotes whose internal interpretation is performed
/// by the rule itself
///
class lexer final {
public:
  ///
  /// @brief Contructor of lexer
  /// @param[in] input input expression to parse
  ///
  explicit lexer(const std::string &input);

  ///
  /// @brief      Iterate over its input string
  /// @return     Returns next parsed token (END for last one)
  /// @throw      Exception on syntax error or
  ///             out of bound request
  ///
  token next();

private:
  token rule();

  std::string m_in;
  std::string::const_iterator m_it;
  std::stack<char> m_parenth;
  bool m_is_end{false};
};
} // namespace n4