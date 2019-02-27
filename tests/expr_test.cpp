#include "gtest/gtest.h"

#include "nforce/core/except.h"
#include "nforce/expr.h"

using namespace n4;

TEST(expr_test, interpret_and) {
  binary_gen_expr<binary_op_type::AND> expr;
  expr.set_left_op(std::make_unique<rule_expr>([] { return true; }));
  expr.set_right_op(std::make_unique<rule_expr>([] { return true; }));

  EXPECT_TRUE(expr.interpret());

  expr.set_right_op(std::make_unique<rule_expr>([] { return false; }));
  EXPECT_FALSE(expr.interpret());
}

TEST(expr_test, interpret_or) {
  binary_gen_expr<binary_op_type::OR> expr;
  expr.set_left_op(std::make_unique<rule_expr>([] { return true; }));
  expr.set_right_op(std::make_unique<rule_expr>([] { return true; }));

  EXPECT_TRUE(expr.interpret());

  expr.set_right_op(std::make_unique<rule_expr>([] { return false; }));
  EXPECT_TRUE(expr.interpret());

  expr.set_left_op(std::make_unique<rule_expr>([] { return false; }));
  EXPECT_FALSE(expr.interpret());
}

TEST(expr_test, interpret_not) {
  unary_not_expr expr;
  expr.set_op(std::make_unique<rule_expr>([] { return true; }));
  EXPECT_FALSE(expr.interpret());

  expr.set_op(std::make_unique<rule_expr>([] { return false; }));
  EXPECT_TRUE(expr.interpret());
}

TEST(expr_test, interpret_bad_binary) {
  binary_gen_expr<binary_op_type::AND> expr;
  expr.set_left_op(std::make_unique<rule_expr>([] { return false; }));

  EXPECT_THROW(expr.interpret(), nexcept);

  binary_gen_expr<binary_op_type::OR> expr2;
  expr2.set_right_op(std::make_unique<rule_expr>([] { return false; }));

  EXPECT_THROW(expr2.interpret(), nexcept);
}

TEST(expr_test, interpret_bad_unary) {
  unary_not_expr expr;
  EXPECT_THROW(expr.interpret(), nexcept);
}

TEST(expr_test, interpret_bad_rule) {
  rule_expr expr;
  EXPECT_THROW(expr.interpret(), nexcept);
}

//-------------------------------------
// Entry point

int expr_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "expr_test*";

  return RUN_ALL_TESTS();
}