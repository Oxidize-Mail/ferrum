//
// Created by ghostvox on 9/30/26.
//
#include "utils/rep.h"

#include <cmath>
#include <iostream>

#include "../include/config.h"

auto calculate_rep(int chain_length, config::Config cfg) -> int

{
  double total_rep = 0;
  int base = cfg.config["settings"]["base_rep"].value<double>().value_or(10);
  std::cout << base << std::endl;
  double rep_scale =
      cfg.config["settings"]["rep_scale"].value<double>().value_or(0.98);

  std::cout << rep_scale << std::endl;
  for (int i = 0; i < chain_length; i++) {
    total_rep += base * pow(rep_scale, i);
  }
  return total_rep;
}
