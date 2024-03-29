#include "cnc_steiner_tree.h"

#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

#include "state_utils.h"
#include "tree.h"

using namespace std;
const unsigned long long int INF = 1000000;

namespace {
typedef unsigned long long ull;
// For bag t: number of edges -> f -> weight -> how many solutions mod 2.
typedef vector<vector<unordered_map<size_t, bool>>> dynamic_results;
}  // namespace

void add_value(dynamic_results& vec, int a, int b, int c, ull val) {
  if (val % 2 == 0) return;
  // Keep only values that modulo 2 give 1.
  if (vec[a][b][c] % 2 == 1) {
    vec[a][b].erase(c);
    return;
  }
  vec[a][b][c] = 1;
}

dynamic_results recursive_cnc_steiner_tree(int l, Bag* bag) {
  dynamic_results vec(l + 2);

  if (bag == nullptr) return vec;
  // Firstly compute partial results for subtrees.
  auto left = std::move(recursive_cnc_steiner_tree(l, bag->left));
  auto right = std::move(recursive_cnc_steiner_tree(l, bag->right));

  State state(bag->nodes, 3);

  for (int j = 0; j <= l; j++) {
    vec[j] = vector<unordered_map<size_t, bool>>(pow(3, bag->nodes.size() + 1));

    if (bag->type == Bag::LEAF) {
      if (j == 0) vec[j][0][0] = 1;
      continue;
    }
    // Root.
    if (bag->type == Bag::FORGET_NODE && state.nodes_.size() == 0) {
      for (auto& weight : left[j][0]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }

      for (auto& weight : left[j][1]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }

      for (auto& weight : left[j][2]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }
      continue;
    }

    for (auto it = state.begin(); it != state.end(); ++it) {
      unsigned long long it_hash = *it;
      switch (bag->type) {
        case Bag::INTRODUCE_NODE: {
          // Terminals have to be taken
          if (bag->introduced_node.terminal &&
              it.GetMapping(bag->introduced_node.value) == 0) {
            vec[j][it_hash].clear();
            break;
          }
          // v0 must be in V^1.
          if (bag->introduced_node.value == 0 &&
              it.GetMapping(bag->introduced_node.value) != 1) {
            vec[j][it_hash].clear();
            break;
          }

          auto hash_without_node =
              it.GetAssignmentHashWithoutNode(bag->introduced_node.value);
          for (auto& weight : left[j][hash_without_node]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }
          break;
        }
        case Bag::FORGET_NODE: {
          auto hash_with_node = it.GetHashWithNode(bag->forgotten_node.value);
          for (auto& weight : left[j][hash_with_node.val_0]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }

          for (auto& weight : left[j][hash_with_node.val_1]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }

          for (auto& weight : left[j][hash_with_node.val_2]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }
          break;
        }
        case Bag::MERGE: {
          for (int i = 0; i <= j; i++) {
            for (auto& weight1 : left[i][it_hash]) {
              if (weight1.second % 2 == 0) continue;
              for (auto& weight2 : right[j - i][it_hash]) {
                add_value(vec, j, it_hash, weight1.first + weight2.first,
                          weight1.second * weight2.second);
              }
            }
          }
          break;
        }
        case Bag::INTRODUCE_EDGE: {
          vec[j][it_hash] = left[j][it_hash];
          // id_1, id_2 refer to sets that introduced edge's endpoints belong
          // to.
          int id_1 = it.GetMapping(bag->introduced_edge.first.value);
          int id_2 = it.GetMapping(bag->introduced_edge.second.value);
          // If the edge may be taken to a solution, i.e. number of all edges is
          // at least 1 and both edge endpoints belong to the same side of a
          // cut.
          if (j > 0 && id_1 == id_2 && id_1 > 0) {
            for (auto& weight : left[j - 1][it_hash]) {
              add_value(vec, j, it_hash, weight.first + bag->edge_weight,
                        weight.second);
            }
          }
          break;
        }
      }
    }
  }
  return vec;
}

unsigned long long CnCSteinerTree::Compute() {
  dynamic_results vec = recursive_cnc_steiner_tree(this->l, this->tree->root);
  for (int i = 0; i <= this->l; i++) {
    for (auto& weight : vec[i][0]) {
      if (weight.second % 2 == 1) return i;
    }
  }
  return INF;
}