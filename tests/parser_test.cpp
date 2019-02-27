#include <regex>

#include "gtest/gtest.h"

#include "nforce/core/except.h"
#include "nforce/expr.h"
#include "nforce/lexer.h"
#include "nforce/parser.h"

using namespace n4;

namespace {
struct context {
  std::string tag;
};

class tag_rule {
public:
  explicit tag_rule(const context &ctxt) : _ctxt{ctxt} {}

  bool do_handle(const std::string &str) const {
    return std::regex_match(str, std::regex{"tag=.*"});
  }

  bool interpret(const std::string &str) const {
    std::regex reg{"tag=(.*)"};
    std::smatch matches;

    if (!std::regex_search(str, matches, reg) || matches.size() != 2) {
      throw nexcept("[nforce] bad rule 1", status_type::INTERNAL_ERROR);
    }

    try {
      reg = matches[1].str();
    } catch (const std::regex_error &) {
      throw nexcept("[nforce] bad rule 1", status_type::INTERNAL_ERROR);
    }

    return std::regex_match(_ctxt.tag, reg);
  }

private:
  const context &_ctxt;
};

struct parser_test : public ::testing::Test {
  parser_test() : rule{ctxt} {
    handler = std::make_pair(
        [&rule = this->rule](const auto &str) { return rule.do_handle(str); },
        [&rule = this->rule](const auto &str) { return rule.interpret(str); });
  }

  context ctxt;
  tag_rule rule;
  parser::rule_handler handler;
};
} // namespace

TEST_F(parser_test, build_main) {
  lexer lexer{"'tag=t1' | 'tag=t2'"};
  parser parser{lexer, std::vector<parser::rule_handler>{handler}};

  std::unique_ptr<expr> expr;
  EXPECT_NO_THROW(expr = parser.build());
  EXPECT_TRUE(expr);

  ctxt.tag = "t1";
  EXPECT_TRUE(expr->interpret());

  ctxt.tag = "t2";
  EXPECT_TRUE(expr->interpret());

  ctxt.tag = "t3";
  EXPECT_FALSE(expr->interpret());
}

TEST_F(parser_test, build_advanced) {
  lexer lexer{"'tag=t[0-9]' | 'tag=r([a-z]*)'"};
  parser parser{lexer, std::vector<parser::rule_handler>{handler}};

  std::unique_ptr<expr> expr;
  EXPECT_NO_THROW(expr = parser.build());
  EXPECT_TRUE(expr);

  ctxt.tag = "t1";
  EXPECT_TRUE(expr->interpret());

  ctxt.tag = "t21";
  EXPECT_FALSE(expr->interpret());

  ctxt.tag = "t";
  EXPECT_FALSE(expr->interpret());

  ctxt.tag = "r";
  EXPECT_TRUE(expr->interpret());

  ctxt.tag = "r125";
  EXPECT_FALSE(expr->interpret());

  ctxt.tag = "rabsd";
  EXPECT_TRUE(expr->interpret());
}

TEST_F(parser_test, build_nohandler) {
  lexer lexer{"'tag=t1' | 'name=t2'"};
  parser parser{lexer, std::vector<parser::rule_handler>{handler}};

  std::unique_ptr<expr> expr;
  EXPECT_THROW(expr = parser.build(), nexcept);
}

//-------------------------------------
// Entry point

int parser_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "parser_test*";

  return RUN_ALL_TESTS();
}