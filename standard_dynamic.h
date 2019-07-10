#pragma once

#include "tree.h"

class StandardDynamic {
 public:
  StandardDynamic(Tree* tree) : tree(tree) {}
  unsigned long long Compute();
  Tree* tree;
};