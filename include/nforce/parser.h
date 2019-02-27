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

#include <functional>
#include <string>
#include <vector>

#include "nforce/lexer.h"

namespace n4 {
class expr;

///
/// @brief Parse and evaluate expression
///
class parser final {
public:
  using checker_cb = std::function<bool(const std::string &)>;
  using handler_cb = std::function<bool(const std::string &)>;

  using rule_handler = std::pair<checker_cb, handler_cb>;

  ///
  /// @brief Contructor of parser
  /// @param[in] lexer
  ///
  explicit parser(lexer &lexer, std::vector<rule_handler> &&handlerList);

  ///
  /// @brief Evaluate expression
  /// @return expression to evaluate
  ///
  /// @warning The handlers must stay valid for the expression to be valid
  ///
  std::unique_ptr<expr> build();

private:
  /// Unit functions for recursive descent parsing
  void expression();
  void eprime();
  void term();
  void tprime();
  void factor();
  void binary(std::unique_ptr<binary_expr>);
  void unary(std::unique_ptr<unary_expr>);
  void rule();

  std::vector<rule_handler> m_handlers;
  lexer &m_lex;
  std::unique_ptr<expr> m_root;
  token m_curr{token_type::END, std::nullopt};
};
} // namespace n4