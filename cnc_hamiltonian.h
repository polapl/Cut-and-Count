#pragma once

#include "state_utils.h"
#include "tree.h"

class CnCHamiltonian {
 public:
  CnCHamiltonian(Tree* tree) : tree(tree) {}
  bool Compute();
  Tree* tree;
};
