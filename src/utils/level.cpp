
//
// Created by brinhasasvlin on 10/08/26.
//
#include "utils/level.h"

#include <cmath>

#include "../include/config.h"

auto level_from_xp(int xp, config::Config cfg) -> int {
  double base =
      cfg.config["settings"]["base_exp"].value<double>().value_or(10);
  double exp_scale =
      cfg.config["settings"]["exp_scale"].value<double>().value_or(1.2);

  // Walk up from level 1 instead of using the closed-form log inverse, which
  // can land just below an exact threshold due to floating-point error.
  double xp_left = xp;
  int level = 1;
  while (xp_left >= base * std::pow(exp_scale, level)) {
    xp_left -= base * std::pow(exp_scale, level);
    ++level;
  }

  return level;
}
