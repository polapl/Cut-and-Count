#pragma once

#include "tree.h"

class Dynamic {
 public:
  Dynamic(Tree* tree, int l) : tree(tree), l(l) {}
  unsigned long long Compute();
  Tree* tree;
  int l;
};
