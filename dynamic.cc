#include "dynamic.h"

#include <cmath>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <map>

#include "tree.h"

using namespace std;
const unsigned long long int INF = 1000000;

// Given a set, class Set is used to iterate through all assignments
// of its elements to 0,1 or 2.
// In terms of cut & cout algoritm:
// 0 ~= isolated Node
// 1 ~= partial solution in V1
// 2 ~= partial solution in V2 

class Set {
  public:
  class Iterator {
    public:
    void operator++() {
      for(auto& it : m_) {
        last_ = true;
        if (it.second < 2) {
          it.second++;
          last_ = false;
          break;
        }
        it.second = 0;
      }
    }

    long long int operator*() const {
      return hash();
    }

    long long int hash_with_node(int id, int val) {
      m_[id] = val;
      auto res = hash();
      m_.erase(id);
      return res;
    }

    long long int hash_without_node(int id) {
      int val = m_[id];
      m_.erase(id);
      auto res = hash();
      m_[id] = val;
      return res;
    }

    int get_value(int id) {
      return m_[id];
    }

    bool operator==(const Iterator& it) const {
      return **this == *it; // not nice but works
    }

    bool operator!=(const Iterator& it) const {
      return **this != *it; // not nice but works
    }

    private:
      Iterator(bool last) : m_(), last_(last) {}
      map<int, int> m_;
      bool last_;
      friend class Set;
      long long int hash() const {
        if (last_) return -1;
        int hash = 0;
        int pow3 = 1;
        for(auto& it : m_) {
          hash += it.second * pow3;
          pow3 = pow3*3;
        }
        return hash;
      }
  };

  Set(const vector<Node>& nodes) : nodes_(nodes) {}

  vector<Node> nodes_;
  
  Iterator begin() {
    //cout << "begin()\n";
    if (nodes_.empty()) {
      return Iterator(true);
    }

    Iterator iterator(false);
    for (const auto& it : nodes_) {
      iterator.m_[it.value] = 0;
    }
    return iterator;
  }
  Iterator end() {
    return Iterator(true);
  }
};

namespace {
typedef unsigned long long ull;
// Bag id -> number of edges -> f -> weight -> how many solutions
typedef vector<vector<unordered_map<size_t, unsigned long long>>> dynamic_results;
}  // namespace

void add_value(dynamic_results& vec, int a, int b, int c, ull val) {
  //if (val == 0) return;
  if (val % 2 == 0) return;
  if (vec[a][b][c] % 2 == 1) {
    vec[a][b].erase(c);
    return;
  }
  vec[a][b][c] = 1;
}

dynamic_results recursive(int k, int l, Bag* bag) {
  dynamic_results vec(l + 2);

  if(bag == nullptr) return vec;
  // Firstly compute partial results for subtrees.
  auto left = std::move(recursive(k, l, bag->left));
  auto right = std::move(recursive(k, l, bag->right));

  Set set(bag->nodes);

  for(int j=0; j<=l; j++){
    vec[j] = vector<unordered_map<size_t, unsigned long long>>(pow(3,bag->nodes.size() + 1));

    if (bag->type == Bag::LEAF) {
      if (j == 0) vec[j][0][0] = 1;
      continue;
    }
    if (bag->type == Bag::FORGET_NODE && set.nodes_.size() == 0) {
      for (auto& weight: left[j][0]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }
      
      for (auto& weight: left[j][1]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }
      
      for (auto& weight: left[j][2]) {
        add_value(vec, j, 0, weight.first, weight.second);
      }
      continue;
    }

    for(auto it = set.begin(); it != set.end(); ++it) {
      unsigned long long it_hash = *it;
      switch(bag->type) {
        case Bag::INTRODUCE_NODE:
        {          
          // Terminals have to be taken
          if (bag->introduced_node.terminal && 
              it.get_value(bag->introduced_node.value) == 0) {

            vec[j][it_hash].clear();
            break;
          }
          // v1
          if (bag->introduced_node.value == 0 &&
              it.get_value(bag->introduced_node.value) != 1) {
            vec[j][it_hash].clear();
            break;
          }

          auto hash_without_node = it.hash_without_node(bag->introduced_node.value);
          for (auto& weight: left[j][hash_without_node]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }
          break;
        }
        case Bag::FORGET_NODE:
        {
          auto hash_with_node = it.hash_with_node(bag->forgotten_node.value, 0);
          for (auto& weight: left[j][hash_with_node]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }
          
          hash_with_node = it.hash_with_node(bag->forgotten_node.value, 1);
          for (auto& weight: left[j][hash_with_node]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }

          hash_with_node = it.hash_with_node(bag->forgotten_node.value, 2);
          for (auto& weight: left[j][hash_with_node]) {
            add_value(vec, j, it_hash, weight.first, weight.second);
          }   
          break;
        }
        case Bag::MERGE:
        {
          for (int i=0; i<=j; i++) {
            for(auto& weight1: left[i][it_hash]) {
              if (weight1.second % 2 == 0) continue;
              for(auto& weight2: right[j-i][it_hash]) {
                add_value(vec, j, it_hash, weight1.first + weight2.first, weight1.second * weight2.second);
              }
            }
            
          }
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          vec[j][it_hash] = left[j][it_hash];
          int id_1 = it.get_value(bag->introduced_edge.first.value);
          int id_2 = it.get_value(bag->introduced_edge.second.value);
          if (j>0 && id_1 == id_2 && id_1 > 0) {
            for (auto& weight: left[j-1][it_hash]) {
              add_value(vec, j, it_hash, weight.first + bag->edge_weight, weight.second);
            }
          }
          break;
        }
      } 
    }
  }
  return vec;
}

unsigned long long Dynamic::Compute() {  
  dynamic_results vec = recursive(this->tree->GetTreeWidth(), this->l, this->tree->root);
  for(int i=0; i <= this->l; i++) {
    for (auto& weight: vec[i][0]) {
      if (weight.second % 2 == 1)
        return i;
    }
  }
  return INF;
}
