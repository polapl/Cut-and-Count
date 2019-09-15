#pragma once

#include "tree.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

#define SM_HASH(a, b) (((a) << 3) | b)
#define INSERT_BLK(h, n) (((h) << 6) | n)
#define GET_K(h, n) (((h) >> ((n)*6 + 3)) & 7)
#define GET_V(h, n) (((h) >> ((n * 6))) & 7)
#define NOT_FOUND (static_cast<size_t>(-1))

typedef unsigned long long hash_t;

hash_t h_make_window(hash_t hsh, int indx);

hash_t h_close_window(hash_t hsh, int indx);

hash_t h_insert_at(hash_t hsh, size_t index, int a, int b);

size_t h_len(hash_t hsh);

size_t h_find_key(hash_t hsh, int key);

void h_insert(hash_t* hsh, int a, int b);

void h_remove(hash_t* hsh, int a);

void h_print(hash_t h);

struct HashWithNodeValues {
  unsigned long long val_0;
  unsigned long long val_1;
  unsigned long long val_2;
};

// Given a set, class State is used to iterate through all assignments
// of its elements to 0, 1, .., s.
// In terms of Steiner Tree Cut & Cout algoritm:
//  0 ~= isolated Node
//  1 ~= partial solution in V1
//  2 ~= partial solution in V2
// In terms of Hamiltonian Cycle Cut & Cout algorithm:
//  0 ~= isolated Node
//  1 ~= Node with one incident edge, in V1
//  2 ~= Node with two incidents edges
//  3 ~= Node with one incident edge, in V2
class State {
 public:
  class Iterator {
   public:
    void operator++();

    long long int operator*() const;

    bool operator==(const Iterator& it) const;

    bool operator!=(const Iterator& it) const;

    int GetMapping(int id);

    int GetIdUsingIdx(int idx);

    set<int> GetAllOnesIndexes();

    set<unsigned long long> GetAllMatchingsHashes(set<int> ones);

    set<unsigned long long> GetAllMatchingsHashes();

    HashWithNodeValues GetHashWithNode(int id);

    unsigned long long GetAssignmentHashWithNode(int id, int val);

    unsigned long long GetAssignmentHashWithoutNode(int id);

    vector<unsigned long long> GetAssignmentHashWithEdge(int id1, int id2);

    vector<int> GetAssignmentHashDiffWithEdge(int val1, int val2, int pow1, int pow2, bool v0);

   private:
    Iterator(bool last, unsigned int s);

    map<int, int> m_;
    bool last_;
    unsigned int s_;
    friend class State;

    long long int hash() const;
  };

  static bool op(const Node& i, const Node& j) { return (i.value < j.value); }

  State(const vector<Node>& nodes, unsigned int assignments);

  vector<Node> nodes_;
  unsigned int assignments_;

  Iterator begin();

  Iterator end();

  pair<int, int> GetEdgeIndexes(const Node& a, const Node& b);

  hash_t h_without_node(hash_t hsh, int val);

  int GetIdxUsingId(int id);
};
