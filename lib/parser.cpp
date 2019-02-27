#include <algorithm>

#include "nforce/core/except.h"
#include "nforce/expr.h"
#include "nforce/parser.h"

//
// Grammar to be parsed:
//
// Goal     ->  Expr
// Expr     ->  Term Expr'
// Expr'    ->  & Term Expr'
//          ->  | TermExpr1
//          ->  | 0
// Term     ->  Factor Term'
// Term'    ->  ! Factor Term'
//          ->  | 0
// Factor   -> (Expr)
//          -> rule

namespace n4 {
void parser::expression() {
  // expr -> term expr'
  this->term();
  this->eprime();
}

void parser::eprime() {
  // expr' -> | term expr'
  // expr' -> & term expr'
  if (m_curr.first == token_type::OR) {
    this->binary(std::make_unique<binary_gen_expr<binary_op_type::OR>>());
  } else if (m_curr.first == token_type::AND) {
    this->binary(std::make_unique<binary_gen_expr<binary_op_type::AND>>());
  } else if (m_curr.first == token_type::RIGHT ||
             m_curr.first == token_type::END) // First+
  {
    return;
  } else {
    throw nexcept("[nforce] invalid eprime parsing", status_type::BAD_PARSE);
  }
}

void parser::term() {
  // term -> factor term'
  this->factor();
  this->tprime();
}

void parser::tprime() {
  // term' -> ! factor term'
  if (m_curr.first == token_type::NOT) {
    this->unary(std::make_unique<unary_not_expr>());
  } else if (m_curr.first == token_type::RIGHT ||
             m_curr.first == token_type::END ||
             m_curr.first == token_type::AND ||
             m_curr.first == token_type::OR) // First+
  {
    return;
  } else {
    throw nexcept("[nforce] invalid tprime parsing", status_type::BAD_PARSE);
  }
}

void parser::factor() {
  // factor -> (expr)
  // factor -> rule
  if (m_curr.first == token_type::LEFT) {
    m_curr = m_lex.next();
    this->expression();
    m_curr = m_lex.next();
  } else if (m_curr.first == token_type::RULE) {
    this->rule();
  } else {
    throw nexcept("[nforce] invalid factor parsing", status_type::BAD_PARSE);
  }
}

void parser::binary(std::unique_ptr<binary_expr> exp) {
  m_curr = m_lex.next();
  exp->set_left_op(std::move(m_root));
  this->term();
  this->eprime();
  exp->set_right_op(std::move(m_root));
  m_root = std::move(exp);
}

void parser::unary(std::unique_ptr<unary_expr> exp) {
  m_curr = m_lex.next();
  this->factor();
  this->tprime();
  exp->set_op(std::move(m_root));
  m_root = std::move(exp);
}

void parser::rule() {
  // check if some content is provided
  if (!m_curr.second.has_value()) {
    throw nexcept("[nforce] invalid rule content", status_type::BAD_PARSE);
  }

  // check if it can be handled
  auto hit = std::find_if(std::cbegin(m_handlers), std::cend(m_handlers),
                          [&](const rule_handler &handler) {
                            return handler.first(m_curr.second.value());
                          });

  if (hit == std::cend(m_handlers)) {
    throw nexcept("[nforce] no handler for rule " + m_curr.second.value(),
                  status_type::BAD_PARSE);
  }

  auto rexp = std::make_unique<rule_expr>(
      std::bind(hit->second, m_curr.second.value()));
  m_root = std::move(rexp);
  m_curr = m_lex.next();
}

//-------------------------------------
// Public

parser::parser(lexer &lexer, std::vector<rule_handler> &&handlerList)
    : m_handlers{std::move(handlerList)}, m_lex{lexer} {}

std::unique_ptr<expr> parser::build() {
  m_curr = m_lex.next();
  this->expression();
  return std::move(m_root);
}
} // namespace n4