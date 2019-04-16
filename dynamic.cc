#include "dynamic.h"

#include <utility>
#include <unordered_map>

#include "tree.h"

//int int int -> int

namespace {
typedef std::tuple<int, int, int> t3;
typedef std::unordered_map<t3, Bag*> dynamic_results;
#define dynamic_results vector<vector<vector<int>>>;

typedef long long ll_t;
ll_t pow(const ll_t x, const ll_t y) {
  return y ? (y & 1 ? x : 1) * pow(x * x, y >> 1) : 1;
}
}  // namespace


ll_t add_to_set(ll_t set, int id, int f_id) {
  int id_to_value = pow(3, id) + fx;
  return set + id_to_value;
}

ll_t delete_from_set(ll_t set, int id) {
  int id_to_value = pow(3, id) + (set / pow(3, id) % 3);
  return set - id_to_value;
}

int f_id(ll_t set, int id) {
  return pow(3, id) + (set / pow(3, id) % 3);
}

ll_t bag_nodes_to_global_set(ll_t f) {
  int global_set = 0;
  int id = 0;
  while (f > 0) {
    int f_id = f % 3;
    global_set = add_to_set(global_set, id, f_id);
    f = f/3;
    id++;
  }
  return global_set;
}

void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);
  
  for(int j=0; j<this->l; j++){
    for(ll_t f; f < pow(3, k-1); f++) {
      ll_t f_final = bag_nodes_to_global_set(f);
      switch(bag->type) {
        case LEAF:
        {
          vec[bag->id][j][f_final] = 1;
        }
        case INTRODUCE_NODE:
        {
          vec[bag->id][j][f_final] = vec[bag->left][j][delete_from_set(f_final, introduced_node->value)];
          if (bag->introduced_node->terminal && f_id = 0) {
            vec[bag->id][j][f] = 0;
          }
          break;
        }
        case FORGET_NODE:
        {
          vec[bag->id][j][f] = vec[bag->left][j][add_to_set(f_final, forgotten_node->value, 0)] +
                               vec[bag->left][j][add_to_set(f_final, forgotten_node->value, 1)] +
                               vec[bag->left][j][add_to_set(f_final, forgotten_node->value, 2)];
        }
        case MERGE:
        {
          for (int i=0; i<=j; i++) {

          }
        }
        case INTRODUCE_EDGE:
        {

        }
      } 
    }
  }

}

int Dynamic::Compute() {
  dynamic_results vec = dynamic_results(this->tree_size);
  for(int i=0; i < tree_size; i++) {
    vec[i] = vector<vector<int>>();
    for(int j=0; i<this->l; j++){
      vec[i][j] = vector<int>();
      for(int f; f<pow(3, k- 1); f++) {
        vec[i][j][f] = 0;
      }
    }
  }
  return 0;
}