#pragma once

#include "tree.h"
#include "state_utils.h"

class CnCHamiltonian {
 public:
  CnCHamiltonian(Tree* tree) : tree(tree) {}
  bool Compute();
  Tree* tree;
};
