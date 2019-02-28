#include <algorithm>
#include <cctype>
#include <optional>

#include "nforce/core/except.h"
#include "nforce/lexer.h"

namespace n4 {
//-------------------------------------
// Private

token lexer::rule() {
  auto match =
      std::find_if(++m_it, std::cend(m_in), [](char c) { return c == '\''; });

  if (match == std::cend(m_in)) {
    throw nexcept(std::string("[nforce] no closing \' around rule"),
                  status_type::BAD_SYNTAX);
  }

  if (std::distance(m_it, match) < 2) // at least one character
  {
    throw nexcept(std::string("[nforce] empty rule"), status_type::BAD_SYNTAX);
  }

  auto ruleBegin = m_it;

  // update iterator
  m_it = match;

  return std::make_pair(token_type::RULE,
                        m_in.substr(std::distance(std::cbegin(m_in), ruleBegin),
                                    std::distance(ruleBegin, match)));
}

//-------------------------------------
// Public

lexer::lexer(const std::string &input) : m_in{input} {
  // clean
  m_in.erase(std::remove_if(std::begin(m_in), std::end(m_in),
                            [](unsigned char c) { return std::isspace(c); }),
             std::end(m_in));

  m_it = std::cbegin(m_in);
}

token lexer::next() {
  if (m_is_end) {
    throw nexcept(std::string("[nforce] out of range token search"),
                  status_type::INTERNAL_ERROR);
  }

  if (m_it == std::end(m_in)) {
    // Invalidate iterator
    m_is_end = true;

    if (m_parenth.size()) {
      throw nexcept("[nforce] missing closing )", status_type::BAD_SYNTAX);
    }

    return std::make_pair(token_type::END, std::nullopt);
  }

  token tok;

  switch (*m_it) {
  case '(':
    m_parenth.push('(');
    tok = std::make_pair(token_type::LEFT, std::nullopt);
    break;
  case ')':
    if (m_parenth.size() == 0 || m_parenth.top() != '(') {
      throw nexcept("[nforce] invalid closing )", status_type::BAD_SYNTAX);
    } else {
      m_parenth.pop();
    }
    tok = std::make_pair(token_type::RIGHT, std::nullopt);
    break;
  case '!':
    tok = std::make_pair(token_type::NOT, std::nullopt);
    break;
  case '&':
    tok = std::make_pair(token_type::AND, std::nullopt);
    break;
  case '|':
    tok = std::make_pair(token_type::OR, std::nullopt);
    break;
  case '\'':
    tok = this->rule();
    break;
  default:
    throw nexcept(std::string("[nforce] invalid char [") + *m_it + "]",
                  status_type::BAD_SYNTAX);
  }

  ++m_it;
  return tok;
}
} // namespace n4