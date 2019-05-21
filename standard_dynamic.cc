#include "standard_dynamic.h"

#include "tree.h"

typedef std::vector<std::vector<std::vector<int>>> dynamic_results ;

typedef long long ll_t;
ll_t pow(const ll_t x, const ll_t y) {
  return y ? (y & 1 ? x : 1) * pow(x * x, y >> 1) : 1;
}

//k = max width, l = max edges in Steiner tree
void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);
/*
  for(...) {
    for (...) {
      switch(bag->type) {
        case Bag::LEAF:
        {
          printf("LEAF[%d][%d][%d]\n", bag->id, x, p); 
          break;
        }
        case Bag::INTRODUCE_NODE:
        {
          printf("INTRODUCE_NODE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::FORGET_NODE:
        {
          printf("FORGET_NODE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::MERGE:
        {
          printf("MERGE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          printf("INTRODUCE_EDGE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        default:
          printf("DEFAULT\n");
      } 
    }
  }
*/
}

int StandardDynamic::Compute() {
  return 0;
}
