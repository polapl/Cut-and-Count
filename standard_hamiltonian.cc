#include "standard_hamiltonian.h"

#include "state_utils.h"

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>

typedef unsigned long long ull;
// X_t partition hash -> matching hash -> true iff. valid Hamiltonian trace exists
typedef unordered_map<size_t, map<size_t, bool>> dynamic_results;

bool get_value(const dynamic_results& vec, int idx1, int idx2) {
  auto find_res1 = vec.find(idx1);
  if (find_res1 == vec.end()) return 0;
  auto find_res2 = find_res1->second.find(idx2);
  if (find_res2 == find_res1->second.end()) return 0;
  return find_res2->second;
}

void set_value(dynamic_results& vec, int idx1, int idx2, bool val) {
  if (!val) return;
  vec[idx1][idx2] = true;
}

dynamic_results MergeChildren(Bag* bag, const dynamic_results& left,
                              const dynamic_results& right) {
  dynamic_results vec;

  State state1(bag->left->nodes, 3);
  State state2(bag->right->nodes, 3);

  for (auto it_state1 = state1.begin(); it_state1 != state1.end();
       ++it_state1) {
    unsigned long long it_state1_hash = *it_state1;

    for (auto it_state2 = state2.begin(); it_state2 != state2.end();
         ++it_state2) {
      unsigned long long it_state2_hash = *it_state2;

      int ones1 = 0, ones2 = 0, ones3 = 0;

      unsigned long long res_color = 0;

      bool incorrect_colors = false;

      for (int i = 0, pow = 1, it_color1 = it_state1_hash,
               it_color2 = it_state2_hash;
           i < bag->nodes.size();
           i++, it_color1 /= 3, it_color2 /= 3, pow *= 3) {
        int cur_val1 = it_color1 % 3;
        int cur_val2 = it_color2 % 3;

        if (cur_val1 == 1) ones1++;
        if (cur_val2 == 1) ones2++;

        if ((cur_val1 + cur_val2) > 2) {
          incorrect_colors = true;
          break;
        }
        if (cur_val1 + cur_val2 == 1) ones3++;
        res_color += (cur_val1 + cur_val2) * pow;
      }

      if (incorrect_colors) continue;
      // Following cases are incorrect as for matching we need an even number of vertices.
      if (ones1 % 2 == 1) continue;
      if (ones2 % 2 == 1) continue;
      if (ones3 % 2 == 1) continue;

      // Following three cases are easy as we don't have to merge matchings.
      if (ones1 == 0 && ones2 == 0) {
        bool partial = get_value(left, it_state1_hash, 0) &&
                       get_value(right, it_state2_hash, 0);
        bool current = get_value(vec, res_color, 0);
        set_value(vec, res_color, 0, partial || current);
        continue;
      }
      if (ones1 == 0 && right.find(it_state2_hash) != right.end()) {
        for (const auto& it_m2 : right.at(it_state2_hash)) {
          bool partial = it_m2.second && get_value(left, it_state1_hash, 0);
          bool current = get_value(vec, res_color, it_m2.first);
          set_value(vec, res_color, it_m2.first, partial || current);
        }
        continue;
      }
      if (ones2 == 0 && left.find(it_state1_hash) != left.end()) {
        for (const auto& it_m1 : left.at(it_state1_hash)) {
          bool partial = it_m1.second && get_value(right, it_state2_hash, 0);
          bool current = get_value(vec, res_color, it_m1.first);
          set_value(vec, res_color, it_m1.first, partial || current);
        }
        continue;
      }

      auto all_matchings1 =
          it_state1.GetAllMatchingsHashes();
      auto all_matchings2 = it_state2.GetAllMatchingsHashes();

      // Check whether it_m1 and it_m2 create a cycle.
      for (const auto& it_m1 :
           all_matchings1) {

        for (const auto& it_m2 : all_matchings2) {
          hash_t res_matching = it_m1;

          bool cycle = false;

          int l = h_len(it_m2);
          for (int i = 0; i < l; ++i) {
            int a = GET_K(it_m2, i);
            int b = GET_V(it_m2, i);
            // Find what vertex a is matched with.
            auto find_a = h_find_key(res_matching, a);
            // Find what vertex b is matched with.
            auto find_b = h_find_key(res_matching, b);
            int a2, b2;
            // a is matched with a2.
            if (find_a != NOT_FOUND) {
              auto key = GET_K(res_matching, find_a);
              auto value = GET_V(res_matching, find_a);
              if (a == key)
                a2 = value;
              else
                a2 = key;
            }
            // b is matched with b2.
            if (find_b != NOT_FOUND) {
              auto key = GET_K(res_matching, find_b);
              auto value = GET_V(res_matching, find_b);
              if (b == key)
                b2 = value;
              else
                b2 = key;
            }

            if (find_a != NOT_FOUND && find_b != NOT_FOUND) {
              if (a == b2) {
                cycle = true;
                break;
              }
              h_insert(&res_matching, a2, b2);

              h_remove(&res_matching, a);
              h_remove(&res_matching, b);
              continue;
            }

            // Merge a -> b, a -> a2. Result: a2 -> b 
            if (find_a != NOT_FOUND) {
              h_remove(&res_matching, a);
              h_insert(&res_matching, a2, b);
              continue;
            }
            // Merge a -> b, b -> b2. Result: a -> b2
            if (find_b != NOT_FOUND) {
              h_remove(&res_matching, b);
              h_insert(&res_matching, a, b2);
              continue;
            }
            // Add a -> b when both of them are not matched.
            h_insert(&res_matching, a, b);
          }

          if (cycle) continue;

          bool partial = get_value(left, it_state1_hash, it_m1) &&
                         get_value(right, it_state2_hash, it_m2);
          bool current = get_value(vec, res_color, res_matching);
          set_value(vec, res_color, res_matching, partial || current);
        }
      }
    }
  }
  return vec;
}

bool IsEdgeEndpoint(int x, pair<Node, Node> edge) {
  if (x == edge.first.value) return true;
  if (x == edge.second.value) return true;
  return false;
}

dynamic_results AddEdge(Bag* bag, const dynamic_results& left) {
  dynamic_results vec;

  State state(bag->left->nodes, 3);
  auto edge_idx = state.GetEdgeIndexes(bag->introduced_edge.first,
                                       bag->introduced_edge.second);
  if (edge_idx.first > edge_idx.second) swap(edge_idx.first, edge_idx.second);

  for (auto it_state = state.begin(); it_state != state.end(); ++it_state) {
    unsigned long long it_state_hash = *it_state;

    if (left.find(it_state_hash) != left.end()) {
      for (const auto& prev_m : left.at(it_state_hash)) {
        set_value(vec, it_state_hash, prev_m.first, prev_m.second);
      }
    }

    unsigned long long res_color = 0;

    bool incorrect_colors = false;
    int ones = 0;
    // Check whether degrees are smaller or equal 2 after adding edge.
    for (int i = 0, pow = 1, it_color = it_state_hash; i < bag->nodes.size();
         i++, pow *= 3, it_color /= 3) {
      int cur_val = it_color % 3;

      if (cur_val == 1) ones++;

      if (cur_val > 1 &&
          IsEdgeEndpoint(it_state.GetIdUsingIdx(i), bag->introduced_edge)) {
        incorrect_colors = true;
        break;
      }
      res_color +=
          (IsEdgeEndpoint(it_state.GetIdUsingIdx(i), bag->introduced_edge)
               ? cur_val + 1
               : cur_val) *
          pow;
    }

    if (incorrect_colors) continue;
    if (ones % 2 == 1) continue;

    const auto& all_matchings = it_state.GetAllMatchingsHashes();

    // Similar as for merging.
    for (const auto& it_m : all_matchings) {
      hash_t res_matching = 0;
      h_insert(&res_matching, edge_idx.first, edge_idx.second);

      bool cycle = false;

      int l = h_len(it_m);
      for (int i = 0; i < l; ++i) {
        int a = GET_K(it_m, i);
        int b = GET_V(it_m, i);
        auto find_a = h_find_key(res_matching, a);
        auto find_b = h_find_key(res_matching, b);
        int a2, b2;

        if (find_a != NOT_FOUND) {
          auto key = GET_K(res_matching, find_a);
          auto value = GET_V(res_matching, find_a);
          if (a == key)
            a2 = value;
          else
            a2 = key;
        }
        if (find_b != NOT_FOUND) {
          auto key = GET_K(res_matching, find_b);
          auto value = GET_V(res_matching, find_b);
          if (b == key)
            b2 = value;
          else
            b2 = key;
        }

        if (find_a != NOT_FOUND && find_b != NOT_FOUND) {
          if (a == b2) {
            cycle = true;
            break;
          }
          h_insert(&res_matching, a2, b2);

          h_remove(&res_matching, a);
          h_remove(&res_matching, b);
          continue;
        }

        if (find_a != NOT_FOUND) {
          h_remove(&res_matching, a);
          h_insert(&res_matching, a2, b);
          continue;
        }
        if (find_b != NOT_FOUND) {
          h_remove(&res_matching, b);
          h_insert(&res_matching, a, b2);
          continue;
        }
        h_insert(&res_matching, a, b);
      }

      if (cycle) continue;

      bool partial = get_value(left, it_state_hash, it_m);
      bool current = get_value(vec, res_color, res_matching);
      set_value(vec, res_color, res_matching, partial || current);
    }
  }
  return vec;
}

dynamic_results ForgetNode(Bag* bag, const dynamic_results& left) {
  dynamic_results vec;
  State state(bag->left->nodes, 3);
  for (auto it = state.begin(); it != state.end();
       ++it) {

    if (it.GetMapping(bag->forgotten_node.value) != 2) continue;
    auto hash_c_wo_n =
        it.GetAssignmentHashWithoutNode(bag->forgotten_node.value);

    set<int> ones = it.GetAllOnesIndexes();
    if (ones.size() % 2 == 1) continue;

    bool current = get_value(vec, hash_c_wo_n, 0);
    bool partial = get_value(left, *it, 0);

    set_value(vec, hash_c_wo_n, 0, current || partial);

    if (ones.size() == 0) continue;

    const auto& all_matchings = it.GetAllMatchingsHashes(ones);

    for (const auto& m : all_matchings) {
      auto m_wo_n = state.h_without_node(m, bag->forgotten_node.value);

      bool partial = get_value(left, *it, m);
      bool current = get_value(vec, hash_c_wo_n, m_wo_n);
      set_value(vec, hash_c_wo_n, m_wo_n, current || partial);
    }
  }
  return vec;
}

dynamic_results AddNode(Bag* bag, const dynamic_results& left) {
  dynamic_results vec;
  State state(bag->nodes, 3);
  for (auto it = state.begin(); it != state.end();
       ++it) {

    if (it.GetMapping(bag->introduced_node.value) != 0) continue;
    auto hash_c_wo_n =
        it.GetAssignmentHashWithoutNode(bag->introduced_node.value);

    set<int> ones = it.GetAllOnesIndexes();
    if (ones.size() % 2 == 1) continue;

    bool current = get_value(vec, *it, 0);
    bool partial = get_value(left, hash_c_wo_n, 0);
    set_value(vec, *it, 0, current || partial);

    if (ones.size() == 0) continue;

    const auto& all_matchings = it.GetAllMatchingsHashes(ones);

    for (const auto& m : all_matchings) {
      auto m_wo_n = state.h_without_node(m, bag->introduced_node.value);

      bool current = get_value(vec, *it, m);
      bool partial = get_value(left, hash_c_wo_n, m_wo_n);

      set_value(vec, *it, m, current || partial);
    }
  }
  return vec;
}

dynamic_results DynamicStandardHamiltonian(Bag* bag) {
  dynamic_results vec;

  if (bag == nullptr) return vec;

  if (bag->type == Bag::LEAF) {
    vec[0][0] = true;
    return vec;
  }

  // Firstly compute partial results for subtrees.
  auto left = std::move(DynamicStandardHamiltonian(bag->left));
  auto right = std::move(DynamicStandardHamiltonian(bag->right));

  if (bag->type == Bag::MERGE) {
    vec = MergeChildren(bag, left, right);
    return vec;
  }

  if (bag->type == Bag::INTRODUCE_EDGE) {
    vec = AddEdge(bag, left);
    return vec;
  }

  if (bag->type == Bag::FORGET_NODE) {
    vec = ForgetNode(bag, left);
    return vec;
  }

  if (bag->type == Bag::INTRODUCE_NODE) {
    vec = AddNode(bag, left);
    return vec;
  }
}

bool StandardHamiltonian::Compute() {
  dynamic_results vec = DynamicStandardHamiltonian(this->tree->root);
  if (vec[4][1]) return true;
  return false;
}