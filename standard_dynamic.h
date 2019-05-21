#pragma once

#include "tree.h"

class StandardDynamic {
 public:
  StandardDynamic(Tree* tree, int l) : tree(tree), l(l) {}
  int Compute();
  Tree* tree;
  int l;
};