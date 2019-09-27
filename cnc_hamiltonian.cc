#include "cnc_hamiltonian.h"

#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

namespace {
typedef unsigned long long ull;
typedef std::unordered_map<size_t, std::unordered_map<size_t, ull>>
    dynamic_results;
}  // namespace

void add_value(dynamic_results& vec, int a, int b, ull val) {
  if (val % 2 == 0) return;
  if (vec[a][b] % 2 == 1) {
    vec[a].erase(b);
    return;
  }
  vec[a][b] = 1;
}

bool in_first(int a, int b) { return (a == 0 || a == 1) && (b == 0 || b == 1); }

bool in_second(int a, int b) {
  return (a == 0 || a == 3) && (b == 0 || b == 3);
}

const int merge_trans[4][4] = {
    {0, 1, 2, 3},
    {1, 2, -1, -1},
    {2, -1, -1, -1},
    {3, -1, -1, 2},
};

unsigned long long hash_after_trans(unsigned long long h1,
                                    unsigned long long h2, bool v1) {
  if (v1) {
    if (h1 % 4 == 0 && h2 % 4 == 3) return -1;
    if (h1 % 4 == 3 && h2 % 4 == 0) return -1;
  }

  unsigned long long res = 0;
  int pow4 = 1;
  while (h1 > 0 || h2 > 0) {
    int a = h1 % 4;
    int b = h2 % 4;
    if (merge_trans[a][b] < 0) return -1;
    res += pow4 * merge_trans[a][b];
    h1 /= 4;
    h2 /= 4;
    pow4 *= 4;
  }
  return res;
}

dynamic_results merge(Bag* bag, dynamic_results& left, dynamic_results& right) {
  dynamic_results vec;
  bool v1 = (bag->nodes[0] == 0);

  State state(bag->nodes, 4);
  for (auto it1 = state.begin(); it1 != state.end(); ++it1) {
    unsigned long long it_hash1 = *it1;

    for (auto it2 = state.begin(); it2 != state.end(); ++it2) {
      unsigned long long it_hash2 = *it2;
      auto hash_trans = hash_after_trans(it_hash1, it_hash2, v1);

      if (hash_trans == -1) continue;

      for (const auto& weight1 : left[it_hash1]) {
        if (weight1.second % 2 == 0) continue;
        for (const auto& weight2 : right[it_hash2]) {
          add_value(vec, hash_trans, weight1.first + weight2.first,
                    weight1.second * weight2.second);
        }
      }
    }
  }
  return vec;
}

typedef long long ll_t;
ll_t pow(const ll_t x, const ll_t y) {
  return y ? (y & 1 ? x : 1) * pow(x * x, y >> 1) : 1;
}

dynamic_results recursive_cnc_hamiltonian(Bag* bag) {
  dynamic_results vec;

  if (bag == nullptr) return vec;

  // Firstly compute partial results for subtrees.
  auto left = std::move(recursive_cnc_hamiltonian(bag->left));
  auto right = std::move(recursive_cnc_hamiltonian(bag->right));

  if (bag->type == Bag::MERGE) return merge(bag, left, right);

  if (bag->type == Bag::LEAF) {
    vec[0][0] = 1;
    return vec;
  }

  State state(bag->nodes, 4);
  // 0 / 1,3 / 2
  if (bag->type == Bag::FORGET_NODE && state.nodes_.size() == 0) {
    for (auto& weight : left[2]) {
      add_value(vec, 0, weight.first, weight.second);
    }
    return vec;
  }

  if (bag->type == Bag::INTRODUCE_EDGE) {
    int idx1 = state.GetIdxUsingId(bag->introduced_edge.first.value);
    int idx2 = state.GetIdxUsingId(bag->introduced_edge.second.value);
    ll_t pow1 = pow(4, idx1);
    ll_t pow2 = pow(4, idx2);

    for (auto it = state.begin(); it != state.end(); ++it) {
      unsigned long long it_hash = *it;

      for (auto& weight : left[it_hash]) {
        add_value(vec, it_hash, weight.first, weight.second);
      }
      int id_1 = it.GetMapping(bag->introduced_edge.first.value);
      int id_2 = it.GetMapping(bag->introduced_edge.second.value);

      if (in_first(id_1, id_2) || in_second(id_1, id_2)) {
        auto hash_with_edge_diff = it.GetAssignmentHashDiffWithEdge(
            id_1, id_2, pow1, pow2, bag->introduced_edge.first.value == 0);
        for (const auto& hash_with_edge_it : hash_with_edge_diff) {
          for (auto& weight : left[it_hash]) {
            add_value(vec, it_hash + hash_with_edge_it,
                      weight.first + bag->edge_weight, weight.second);
          }
        }
      }
    }
    return vec;
  }

  for (auto it = state.begin(); it != state.end(); ++it) {
    unsigned long long it_hash = *it;

    switch (bag->type) {
      case Bag::INTRODUCE_NODE: {
        if (it.GetMapping(bag->introduced_node.value) != 0) {
          vec[it_hash].clear();
          break;
        }

        auto hash_without_node =
            it.GetAssignmentHashWithoutNode(bag->introduced_node.value);
        for (auto& weight : left[hash_without_node]) {
          add_value(vec, it_hash, weight.first, weight.second);
        }
        break;
      }
      case Bag::FORGET_NODE: {
        auto hash_with_node =
            it.GetAssignmentHashWithNode(bag->forgotten_node.value, 2);
        for (auto& weight : left[hash_with_node]) {
          add_value(vec, it_hash, weight.first, weight.second);
        }
        break;
      }
    }
  }
  return vec;
}

bool CnCHamiltonian::Compute() {
  dynamic_results vec = recursive_cnc_hamiltonian(this->tree->root);
  for (auto& weight : vec[0]) {
    if (weight.second % 2 == 1) {
      return true;
    }
  }
  return false;
}