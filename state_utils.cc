#include "state_utils.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

hash_t h_make_window(hash_t hsh, int indx) {
  hash_t prefix_mask = (1 << (6 * (indx))) - 1;
  return ((hsh >> (6 * indx)) << (6 * (indx + 1))) | (hsh & prefix_mask);
}

hash_t h_close_window(hash_t hsh, int indx) {
  hash_t prefix_mask = (1 << (6 * (indx))) - 1;
  hash_t suffix_mask = (-1) ^ (prefix_mask << 6);
  return (hsh & prefix_mask) | ((hsh & suffix_mask) >> 6);
}

hash_t h_insert_at(hash_t hsh, size_t index, int a, int b) {
  assert(a < b);
  return h_make_window(hsh, index) | (SM_HASH(a, b) << (6 * index));
}

size_t h_len(hash_t hsh) {
  size_t result = 0;
  for (; hsh; ++result, hsh >>= 6)
    ;
  return result;
}

size_t h_find_key(hash_t hsh, int key) {
  int l = h_len(hsh);
  for (int i = 0; i < l; ++i) {
    if (GET_K(hsh, i) == key || GET_V(hsh, i) == key) return i;
  }
  return NOT_FOUND;
}

void h_insert(hash_t* hsh, int a, int b) {
  if (a > b) swap(a, b);
  size_t len = h_len(*hsh);
  int indx = len;
  size_t first_larger = 0;
  for (; indx > 0; --indx) {
    if (GET_K(*hsh, indx - 1) < a) {
      first_larger = indx;
      break;
    }
  }
  *hsh = h_insert_at(*hsh, first_larger, a, b);
}

void h_remove(hash_t* hsh, int a) {
  *hsh = h_close_window(*hsh, h_find_key(*hsh, a));
}

void h_print(hash_t h) {
  int l = h_len(h);
  cout << "$" << oct << h << dec;
  for (int i = 0; i < l; ++i) {
    cout << "|" << GET_K(h, i) << ";" << GET_V(h, i) << "|";
  }
  cout << endl;
}

void State::Iterator::operator++() {
  for (auto& it : m_) {
    last_ = true;
    if (it.second < (s_ - 1)) {
      it.second++;
      last_ = false;
      break;
    }
    it.second = 0;
  }
}

long long int State::Iterator::operator*() const { return hash(); }

bool State::Iterator::operator==(const State::Iterator& it) const {
  return **this == *it;
}

bool State::Iterator::operator!=(const State::Iterator& it) const {
  return **this != *it;
}

HashWithNodeValues State::Iterator::GetHashWithNode(int id) {
  HashWithNodeValues res;
  int hash = 0;
  int pow3 = 1;
  int id_pow3;
  m_[id] = 0;
  for (auto& it : m_) {
    if (it.first == id) id_pow3 = pow3;
    hash += it.second * pow3;
    pow3 = pow3 * 3;
  }
  m_.erase(id);
  res.val_0 = hash;
  res.val_1 = hash + id_pow3;
  res.val_2 = hash + (2 * id_pow3);
  return res;
}

int State::Iterator::GetMapping(int id) { return m_[id]; }

int State::Iterator::GetIdUsingIdx(int idx) {
  int i = 0;
  for (const auto& it : m_) {
    if (i == idx) return it.first;
    i++;
  }
}

set<int> State::Iterator::GetAllOnesIndexes() {
  set<int> res;
  int i = 0;
  for (const auto& it : m_) {
    if (it.second == 1) res.insert(i);
    i++;
  }
  return res;
}

// Tab represents which X_i, X_j u, v belong to after adding an edge uv.
// Tab[k][m] = {i, j} means u was in X_k in t', v was in X_m in t',
// now they are in X_i, X_j respectively.
const vector<vector<int>> add_edge_trans[4][4] = {
    {{{1, 1}, {3, 3}}, {{1, 2}}, {}, {{3, 2}}},
    {{{2, 1}}, {{2, 2}}, {}, {}},
    {{}, {}, {}, {}},
    {{{2, 3}}, {}, {}, {{2, 2}}},
};

set<unsigned long long> State::Iterator::GetAllMatchingsHashes(set<int> ones) {
  set<unsigned long long> res;

  if (ones.size() == 0) {
    res.insert(0);
    return res;
  }

  auto first = *ones.begin();
  ones.erase(first);

  set<int> copy = ones;

  for (auto it : ones) {
    int paired_val = it;
    copy.erase(paired_val);

    set<unsigned long long> partial_set = GetAllMatchingsHashes(copy);

    for (auto& matching : partial_set) {
      auto matching_copy = matching;
      matching_copy = INSERT_BLK(matching_copy, SM_HASH(first, paired_val));
      res.insert(matching_copy);
    }

    copy.insert(paired_val);
  }

  ones.insert(first);
  return res;
}

set<unsigned long long> State::Iterator::GetAllMatchingsHashes() {
  return GetAllMatchingsHashes(GetAllOnesIndexes());
}

unsigned long long State::Iterator::GetAssignmentHashWithNode(int id, int val) {
  m_[id] = val;
  auto res = hash();
  m_.erase(id);
  return res;
}

unsigned long long State::Iterator::GetAssignmentHashWithoutNode(int id) {
  int hash = 0;
  int pow = 1;
  for (auto& it : m_) {
    if (it.first == id) continue;
    hash += it.second * pow;
    pow = pow * s_;
  }
  return hash;
}

vector<int> State::Iterator::GetAssignmentHashDiffWithEdge(int val1, int val2,
                                                           int pow1, int pow2,
                                                           bool v0) {
  vector<int> res;
  for (const auto& it : add_edge_trans[val1][val2]) {
    if (v0 && it[0] == 3) continue;
    res.push_back(pow1 * (it[0] - val1) + (pow2 * (it[1] - val2)));
  }
  return res;
}

State::Iterator::Iterator(bool last, unsigned int s)
    : m_(), last_(last), s_(s) {}

long long int State::Iterator::hash() const {
  if (last_) return -1;
  int hash = 0;
  int pow = 1;
  for (auto& it : m_) {
    hash += it.second * pow;
    pow = pow * s_;
  }
  return hash;
}

State::State(const vector<Node>& nodes, unsigned int assignments)
    : nodes_(nodes), assignments_(assignments) {
  sort(nodes_.begin(), nodes_.end(), op);
}

State::Iterator State::begin() {
  if (nodes_.empty()) {
    return State::Iterator(true, assignments_);
  }

  State::Iterator iterator(false, assignments_);
  for (const auto& it : nodes_) {
    iterator.m_[it.value] = 0;
  }
  return iterator;
}

State::Iterator State::end() { return State::Iterator(true, assignments_); }

pair<int, int> State::GetEdgeIndexes(const Node& a, const Node& b) {
  pair<int, int> res;
  int i = 0;
  for (const auto& n : nodes_) {
    if (n.value == a.value) res.first = i;
    if (n.value == b.value) res.second = i;
    i++;
  }
  return res;
}

hash_t State::h_without_node(hash_t hsh, int val) {
  int idx = 0;
  for (const auto& n : nodes_) {
    if (n.value > val) break;
    idx++;
  }

  size_t l = h_len(hsh);
  hash_t res = 0;
  for (int i = l - 1; i >= 0; i--) {
    int a = GET_K(hsh, i);
    int b = GET_V(hsh, i);
    if (a >= idx) a--;
    if (b >= idx) b--;
    h_insert(&res, a, b);
  }
  return res;
}

int State::GetIdxUsingId(int id) {
  int i = 0;
  for (const auto& it : nodes_) {
    if (it.value == id) return i;
    i++;
  }
}
