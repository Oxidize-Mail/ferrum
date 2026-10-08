//
// Created by ghostvox on 9/30/26.
//
#include <toml++/toml.h>

#include "config.h"
#include "gtest/gtest.h"
#include "utils/rep.h"

TEST(TEST_UTILS, REP_GRANT) {
  toml::table tbl;

  tbl.insert_or_assign("settings.rep_scale", .98);
  tbl.insert_or_assign("settings.base_rep", 10);
  config::Config cfg = config::Config(tbl);
  int gained_rep = calculate_rep(5, cfg);

  EXPECT_EQ(gained_rep, 48);
}
