#pragma once

#include "tree.h"

class StandardHamiltonian {
 public:
  StandardHamiltonian(Tree* tree) : tree(tree) {}
  bool Compute();
  Tree* tree;
};
