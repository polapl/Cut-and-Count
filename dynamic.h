#pragma once

#include "tree.h"

class Dynamic {
 public:
  Dynamic(Tree* tree, int l) : tree(tree), l(l) {}
  int Compute();
  Tree* tree;
  int l;
};
