#pragma once

#include "tree.h"

class CnCSteinerTree {
 public:
  CnCSteinerTree(Tree* tree, int l) : tree(tree), l(l) {}
  unsigned long long Compute();
  Tree* tree;
  int l;
};
