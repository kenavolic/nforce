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
#include <memory>
#include <optional>

#include "nforce/core/except.h"

namespace n4 {
enum class binary_op_type { OR = 0, AND };

///
/// @brief Base expression
///
class expr {
public:
  expr(const expr &) = delete;
  expr &operator=(const expr &) = delete;
  expr() = default;
  virtual ~expr() = default;

  virtual bool interpret() const = 0;
};

class binary_expr : public expr {
public:
  virtual void set_left_op(std::unique_ptr<expr> expr) = 0;
  virtual void set_right_op(std::unique_ptr<expr> expr) = 0;
};

class unary_expr : public expr {
public:
  virtual void set_op(std::unique_ptr<expr> expr) = 0;
};

///
/// @brief Binary boolean expression
///
template <binary_op_type Op> class binary_gen_expr : public binary_expr {
public:
  bool interpret() const override {
    if (!m_op1 || !m_op2) {
      throw nexcept("[nforce] missing binary operand", status_type::BAD_AST);
    }

    if constexpr (Op == binary_op_type::AND) {
      return m_op1->interpret() && m_op2->interpret();
    } else {
      return m_op1->interpret() || m_op2->interpret();
    }
  }

  void set_left_op(std::unique_ptr<expr> expr) override {
    m_op1 = std::move(expr);
  }

  void set_right_op(std::unique_ptr<expr> expr) override {
    m_op2 = std::move(expr);
  }

private:
  std::unique_ptr<expr> m_op1;
  std::unique_ptr<expr> m_op2;
};

///
/// @brief Unary boolean expression
///
class unary_not_expr : public unary_expr {
public:
  bool interpret() const override {
    if (!m_op) {
      throw nexcept("[nforce] missing unary operand", status_type::BAD_AST);
    }

    return !m_op->interpret();
  }

  void set_op(std::unique_ptr<expr> expr) override { m_op = std::move(expr); }

private:
  std::unique_ptr<expr> m_op;
};

///
/// @brief Leaf expression that contains rules to enforce
///
class rule_expr : public expr {
public:
  using interpretor = std::function<bool()>;

  rule_expr() = default;
  explicit rule_expr(interpretor &&i) : m_interpretor{std::move(i)} {}

  void set_interpretor(interpretor &&i) { *m_interpretor = std::move(i); }

  bool interpret() const override {
    if (!m_interpretor.has_value()) {
      throw nexcept("[nforce] missing rule interpretor operand",
                    status_type::BAD_AST);
    }

    return (*m_interpretor)();
  }

private:
  std::optional<interpretor> m_interpretor;
};
} // namespace n4