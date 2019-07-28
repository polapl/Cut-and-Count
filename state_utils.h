#pragma once

#include <map>
#include <vector>
#include <algorithm>

using namespace std;

struct hash_with_012 {
  unsigned long long val_0;
  unsigned long long val_1;
  unsigned long long val_2;
};


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

    hash_with_012 hash_with_node(int id) {
      hash_with_012 res;
      int hash = 0;
      int pow3 = 1;
      int id_pow3;
      m_[id] = 0;
      for(auto& it : m_) {
        if (it.first == id) id_pow3 = pow3;
        hash += it.second * pow3;
        pow3 = pow3*3;
      }
      m_.erase(id);
      res.val_0 = hash;
      res.val_1 = hash + id_pow3;
      res.val_2 = hash + (2*id_pow3);
      return res;
    }

    long long int hash_without_node(int id) {
      int hash = 0;
      int pow3 = 1;
      for(auto& it : m_) {
        if (it.first == id) continue;
        hash += it.second * pow3;
        pow3 = pow3*3;
      }
      return hash;
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


class SSet {
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

    bool operator==(const Iterator& it) const {
      return **this == *it; // not nice but works
    }

    bool operator!=(const Iterator& it) const {
      return **this != *it; // not nice but works
    }

    set<int> give_all_ones_idx() {
    	set<int> res;
    	int i = 0;
    	for(const auto& it : m_) {
    		if (it.second == 1) res.insert(i);
    		i++;
    	}
    	return res;
    }

    map<int, int> give_colors() {
    	return m_;
    }

    int get_color(int idx) {
    	return m_[idx];
    }

    set<map<int, int>> compute_matching(set<int> ones) {
    	set<map<int, int>> res;
    	if (ones.size() == 0) return res;
    	
    	
    	set<int> copy = ones;
    	auto first = ones.begin();
    	
    	ones.erase(first);
    	copy.erase(*first);
    	
    	for(auto it : ones) {
    		copy.erase(it);
    		
    		set<map<int, int>> partial_set = compute_matching(copy);

    		if (partial_set.empty()) {
    			map<int, int> matching;
    			matching[it] = *first;
    			matching[*first] = it;
    			res.insert(matching);
    		}

    		for(auto& matching : partial_set) {
    			auto matching_copy = matching;
    			matching_copy[it] = *first;
    			matching_copy[*first] = it;
    			res.insert(matching_copy);
    		}

    		copy.insert(it);
    	}
    	
    	ones.insert(*first);
    	return res;
    }

    set<map<int, int>> compute_matching() {
    	return compute_matching(give_all_ones_idx());
    }

    long long int hash_c_with_node(int id, int val) {
     	m_[id] = val;
    	auto res = hash();
    	m_.erase(id);
    	return res;
    }
    /*
    long long int hash_c_without_node(int id) {
    	int tmp = m_[id];
    	m_.erase(id);
    	int hash = 0;
    	int pow3 = 1;
    	for(auto& it : m_) {
        	hash += it.second * pow3;
        	pow3 = pow3*3;
      	}
      	m_[id] = tmp;
      	return hash;
    }
	*/

    map<int, int> c_without_node(int id) {
    	map<int, int> res = m_;
    	res.erase(id);
    	return res;
    }

    private:
      Iterator(bool last) : m_(), last_(last) {}
      map<int, int> m_;
      bool last_;
      friend class SSet;
      
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

	static bool op (const Node& i, const Node& j) { return (i.value < j.value); }

	SSet(const vector<Node>& nodes) : nodes_(nodes) { sort(nodes_.begin(), nodes_.end(), op);}

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

	pair<int, int> edge_idx(const Node& a, const Node& b) {
		pair<int, int> res;
		int i = 0;
		for (const auto& n : nodes_) {
			if (n.value == a.value) res.first = i;
			if (n.value == b.value) res.second = i;
			i++;
		}
		return res;
	}

	map<int, int> m_without_node(map<int, int> m, int val) {
		m.erase(val);
		int idx = 0;
		for(const auto& n : nodes_) {
			if (n.value > val) break;
			idx++;
		}
		map<int, int> res;
		for(const auto& p : m) {
			int a = p.first >= idx ? p.first - 1 : p.first;
			int b = p.second >= idx ? p.second - 1 : p.second;
			res[a] = b;
			res[b] = a;
		}
		return res;
	}
};