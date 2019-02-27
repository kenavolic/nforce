#include "gtest/gtest.h"

#include "nforce/core/except.h"
#include "nforce/lexer.h"

using namespace n4;

TEST(lexer_test, next_main) {
  lexer lexer{"(('rule1' &   !'rule2') |   'rule3') & 'rule4'"};

  EXPECT_EQ(lexer.next().first, token_type::LEFT);
  EXPECT_EQ(lexer.next().first, token_type::LEFT);

  auto rule = lexer.next();
  EXPECT_EQ(rule.first, token_type::RULE);
  EXPECT_TRUE(rule.second.has_value());
  EXPECT_STREQ(rule.second.value().c_str(), "rule1");

  EXPECT_EQ(lexer.next().first, token_type::AND);
  EXPECT_EQ(lexer.next().first, token_type::NOT);

  rule = lexer.next();
  EXPECT_EQ(rule.first, token_type::RULE);
  EXPECT_TRUE(rule.second.has_value());
  EXPECT_STREQ(rule.second.value().c_str(), "rule2");

  EXPECT_EQ(lexer.next().first, token_type::RIGHT);
  EXPECT_EQ(lexer.next().first, token_type::OR);

  rule = lexer.next();
  EXPECT_EQ(rule.first, token_type::RULE);
  EXPECT_TRUE(rule.second.has_value());
  EXPECT_STREQ(rule.second.value().c_str(), "rule3");

  EXPECT_EQ(lexer.next().first, token_type::RIGHT);
  EXPECT_EQ(lexer.next().first, token_type::AND);

  rule = lexer.next();
  EXPECT_EQ(rule.first, token_type::RULE);
  EXPECT_TRUE(rule.second.has_value());
  EXPECT_STREQ(rule.second.value().c_str(), "rule4");

  EXPECT_EQ(lexer.next().first, token_type::END);
}

TEST(lexer_test, next_empty) {
  lexer lexer{""};

  EXPECT_EQ(lexer.next().first, token_type::END);
}

TEST(lexer_test, next_overflow) {
  lexer lexer{""};

  EXPECT_EQ(lexer.next().first, token_type::END);
  EXPECT_THROW(lexer.next().first, nexcept);
}

TEST(lexer_test, next_badchar) {
  lexer lexer{"x"};

  EXPECT_THROW(lexer.next().first, nexcept);
}

TEST(lexer_test, next_no_closing_par) {
  lexer lexer{"('rule1'&('rule2')"};

  for (std::size_t i = 0; i < 6; ++i) {
    EXPECT_NO_THROW(lexer.next());
  }

  EXPECT_THROW(lexer.next().first, nexcept);
}

TEST(lexer_test, next_no_opening_par) {
  lexer lexer{"'rule1'&'rule2')"};

  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_NO_THROW(lexer.next());
  }

  EXPECT_THROW(lexer.next().first, nexcept);
}

TEST(lexer_test, next_empty_rule) {
  lexer lexer{"''&'rule2'"};

  EXPECT_THROW(lexer.next().first, nexcept);
}

//-------------------------------------
// Entry point

int lexer_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "lexer_test*";

  return RUN_ALL_TESTS();
}