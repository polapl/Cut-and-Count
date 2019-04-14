#pragma once

#include "tree.h"

class Dynamic {
  Dynamic(Tree* tree, int tree_size, int k, int l) : tree(tree), tree_size(tree_size), k(k), l(l) {}
  int Compute();
  Tree* tree;
  int l;
  int tree_size;
};
