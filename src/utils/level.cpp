
//
// Created by brinhasasvlin on 10/08/26.
//
#include "utils/level.h"

#include <iostream>

#include "../include/config.h"

auto level_from_xp(int xp, config::Config cfg) -> int {
  double xp_left = xp;
  int base = cfg.config["settings"]["base_exp"].value<double>().value_or(10);
  std::cout << base << std::endl;
  double exp_scale =
      cfg.config["settings"]["exp_scale"].value<double>().value_or(0.98);
  std::cout << exp_scale << std::endl;

  int l = 0;
  for (l = 0; xp_left > 0; l++) {
    xp_left -= base * pow(exp_scale, l);
  }

  return l;
}
