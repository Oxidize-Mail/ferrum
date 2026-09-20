//
// Created by ghostvox on 9/17/26.
//
#include "config.h"
#include "gtest/gtest.h"

TEST(ReadToken, TestFerrum) {
  setenv("BOT_TOKEN", "123", 1);
  std::string token = get_bot_token();
  EXPECT_EQ("123", token);
}
