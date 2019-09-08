#include "standard_steiner_tree.h"

#include <cmath>

#include <array>
#include <unordered_map>
#include "disjoint_set.h"
#include "subset_view.h"
#include "tree.h"

using namespace std;

const unsigned long long int INF = 1000000;

typedef vector<unordered_map<size_t, unsigned long long>> dynamic_results;
typedef unsigned long long ull;

unsigned long long get_value(const dynamic_results& vec, int a, int b) {
  auto first_map = vec[a];
  if (first_map.size() == 0) return INF;
  auto second_map = first_map.find(b);
  if (second_map == first_map.end()) return INF;
  return (*second_map).second;
}

void set_value(dynamic_results& vec, int a, int b, unsigned long long val) {
  if (val == INF) return;
  vec[a][b] = val;
}

// Used for Bag of INTRODUCE_EDGE type.
// If we introduce a new edge that connects two different partition subsets,
// we have to merge them.
void merge_child_partitions(const Node& first, const Node& second,
                            const dynamic_results& cur, dynamic_results& parent,
                            Bag* t) {
  SubsetView<Node> sset(t->nodes);
  for (; sset; ++sset) {
    ull sset_hash = sset.hash();

    if (!sset.is_present(first) || !sset.is_present(second)) continue;
    vector<Node> sset_materialize = sset.materialize();
    PartitionView<Node> pset(sset_materialize);
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      int p1 = it.partition(first);
      int p2 = it.partition(second);
      if (p1 == p2) continue;
      int merged = it.merge(p1, p2);
      ull current = get_value(parent, sset_hash, merged);
      ull partial = get_value(cur, sset_hash, it.hash());
      set_value(parent, sset_hash, merged, min(current, partial + 1));
    }
  }
}

// k = max width, l = max edges in Steiner tree
dynamic_results recursive_standard_steiner_tree(int k, Bag* bag) {
  if (bag == nullptr) return dynamic_results{};

  ull size = pow(2, bag->nodes.size() + 1);
  dynamic_results vec(size + 2);

  auto left = std::move(recursive_standard_steiner_tree(k, bag->left));
  auto right = std::move(recursive_standard_steiner_tree(k, bag->right));

  bool one_edge_introduced = false;

  SubsetView<Node> sset(bag->nodes);
  // Iterate through all subsets of bag->nodes
  for (; sset; ++sset) {
    ull sset_hash = sset.hash();

    vector<Node> sset_materialize = sset.materialize();

    PartitionView<Node> pset(sset_materialize);
    // Iterate through all possible partitions
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      ull it_hash = it.hash();

      switch (bag->type) {
        case Bag::LEAF: {
          set_value(vec, sset_hash, it_hash, 0);
          break;
        }
        case Bag::INTRODUCE_NODE: {
          if (bag->introduced_node.terminal &&
              !sset.is_present(bag->introduced_node))
            break;
          if (sset.is_present(bag->introduced_node) &&
              !it.singleton(bag->introduced_node))
            break;
          if (sset.is_present(bag->introduced_node)) {
            int partition_without_node =
                it.remove_singleton(bag->introduced_node);
            unsigned long long partial =
                get_value(left, sset.hash_without_el(bag->introduced_node),
                          partition_without_node);
            set_value(vec, sset_hash, it_hash, partial);
          } else {
            unsigned long long partial = get_value(
                left, sset.hash_without_el(bag->introduced_node), it_hash);
            set_value(vec, sset_hash, it_hash, partial);
          }
          break;
        }
        case Bag::FORGET_NODE: {
          ull partial = get_value(
              left, sset.hash_with_el(bag->forgotten_node, false), it_hash);
          set_value(vec, sset_hash, it_hash, partial);
          int max_part = it.max_partition();

          for (int i = 1; i <= max_part; i++) {
            ull partition_with_node_hash =
                it.add_to_partition_hash(bag->forgotten_node, i);
            ull set_with_node_hash =
                sset.hash_with_el(bag->forgotten_node, true);
            ull current = get_value(vec, sset_hash, it_hash);
            partial =
                get_value(left, set_with_node_hash, partition_with_node_hash);
            set_value(vec, sset_hash, it_hash, min(current, partial));
          }
          break;
        }
        case Bag::MERGE: {
          // part1 and part2 are used to iterate through all possible pairs
          // that:
          // - do not create an cycle
          // - sum up to pset
          // PartitionView<Node> part1 = PartitionView<Node>(sset_materialize);
          // PartitionView<Node> part2 = PartitionView<Node>(sset_materialize);
          // Check whether after merging part1 and part2 we will end up with
          // cycle
          for (auto part_it1 = pset.begin(); part_it1 != pset.end();
               ++part_it1) {
            DisjointSet<int> disjoint_set;
            for (const auto& s_m_it : sset_materialize) {
              disjoint_set.add(s_m_it.value);
            }
            const auto& distribution1 = part_it1.distribution();
            for (const auto& distr_it : distribution1) {
              for (int s = 1; s < distr_it.second.size(); s++) {
                disjoint_set.join(distr_it.second[0].get().value,
                                  distr_it.second[s].get().value);
              }
            }
            for (auto part_it2 = pset.begin(); part_it2 != pset.end();
                 ++part_it2) {
              bool cycle = false;
              DisjointSet<int> disjoint_set_copy = disjoint_set;
              const auto& distribution2 =
                  part_it2.distribution();  // set setow zamiast mapy?
              for (const auto& distr_it : distribution2) {
                for (int s = 1; s < distr_it.second.size(); s++) {
                  if (disjoint_set_copy.find(distr_it.second[0].get().value) ==
                      disjoint_set_copy.find(distr_it.second[s].get().value)) {
                    cycle = true;
                    break;
                  }
                  disjoint_set_copy.join(distr_it.second[0].get().value,
                                         distr_it.second[s].get().value);
                }
              }
              if (!cycle) {
                bool equal = true;
                // Check whether x,y in the same part in pset => find(x) ==
                // find(y)
                auto pset_distribution = it.distribution();
                for (const auto& map_it : pset_distribution) {
                  for (int s = 1; s < map_it.second.size(); s++) {
                    if (disjoint_set_copy.find(map_it.second[0].get().value) ==
                        disjoint_set_copy.find(map_it.second[s].get().value))
                      continue;
                    equal = false;
                  }
                }
                if (!equal) continue;
                map<DisjointSet<int>::Representative, vector<Node>>
                    merged_parts;
                for (const auto& s_m_it : sset_materialize) {
                  merged_parts[disjoint_set_copy.find(s_m_it.value)].push_back(
                      s_m_it);
                }
                for (const auto& merged_parts_it : merged_parts) {
                  for (int s = 1; s < merged_parts_it.second.size(); s++) {
                    Node el1 = merged_parts_it.second[0];
                    Node el2 = merged_parts_it.second[s];
                    if (it.partition(el1) != it.partition(el2)) equal = false;
                  }
                }
                if (!equal) continue;
                ull current = get_value(vec, sset_hash, it_hash);
                ull partial_1 = get_value(left, sset_hash, part_it1.hash());
                ull partial_2 = get_value(right, sset_hash, part_it2.hash());
                set_value(vec, sset_hash, it_hash,
                          min(current, partial_1 + partial_2));
              }
            }
          }
          break;
        }
        case Bag::INTRODUCE_EDGE: {
          // If we cannot take this edge to solution because
          // one of its endpoints is not in the current subset.
          if (!sset.is_present(bag->introduced_edge.first) ||
              !sset.is_present(bag->introduced_edge.second)) {
            ull partial = get_value(left, sset_hash, it_hash);
            set_value(vec, sset_hash, it_hash, partial);
            break;
          }
          int part_a = it.partition(bag->introduced_edge.first);
          int part_b = it.partition(bag->introduced_edge.second);
          // If we cannot take this edge to solution because
          // its endpoints are in different partitions.
          if (part_a != part_b) {
            ull partial = get_value(left, sset_hash, it_hash);
            set_value(vec, sset_hash, it_hash, partial);
            break;
          }
          // When we may take the edge to the solution.
          if (!one_edge_introduced) {
            merge_child_partitions(bag->introduced_edge.first,
                                   bag->introduced_edge.second, left, vec,
                                   bag->left);
            one_edge_introduced = true;
          }
          break;
        }
      }
    }
  }
  return vec;
}

unsigned long long StandardSteinerTree::Compute() {
  const dynamic_results& vec = recursive_standard_steiner_tree(
      this->tree->GetTreeWidth(), this->tree->root);
  unsigned long long res = get_value(vec, 1, 1);
  return res;
}
