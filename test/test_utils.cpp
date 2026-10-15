//
// Created by ghostvox on 9/30/26.
//
#include <toml++/toml.h>

#include "config.h"
#include "gtest/gtest.h"
#include "utils/level.h"
#include "utils/rep.h"

TEST(TEST_UTILS, REP_GRANT) {
  toml::table tbl;

  tbl.insert_or_assign("settings.rep_scale", .98);
  tbl.insert_or_assign("settings.base_rep", 10);
  config::Config cfg = config::Config(tbl);
  int gained_rep = calculate_rep(5, cfg);

  EXPECT_EQ(gained_rep, 48);
}

TEST(TEST_UTILS, LEVELS) {
  toml::table tbl;

  tbl.insert_or_assign("settings",
                       toml::table{{"base_exp", 10}, {"exp_scale", 1.2}});

  config::Config cfg = config::Config(tbl);
  int level = level_from_xp(10, cfg);
  EXPECT_EQ(level, 1);

  level = level_from_xp(250, cfg);
  EXPECT_EQ(level, 10);

  EXPECT_EQ(level_from_xp(0, cfg), 1);
  EXPECT_EQ(level_from_xp(11, cfg), 1);
  EXPECT_EQ(level_from_xp(12, cfg), 2);
  EXPECT_EQ(level_from_xp(455000, cfg), 50);
}
