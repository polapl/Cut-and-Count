#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include <cmath>

using namespace std;

#define unset_bit(v, k) ((v) & (~(1ULL << (k))))

template <typename C, typename A>
bool contains(const C& container, const A& value) {
  return container.find(value) != container.end();
}

template <typename A>
bool contains(const vector<A>& container, const A& value) {
  return find(container.begin(), container.end(), value) != container.end();
}

// Class to iterate through all subsets of a given set.
template <typename T>
class SubsetView {
 public:
  SubsetView(vector<T>& data)
      : mask_(1),
        limit_((static_cast<unsigned long long>(1) << data.size()) - 1),
        data_set_(data) {}

  class iterator {
   public:
    bool operator==(const iterator& it) { return it.mask_ == mask_; }
    bool operator!=(const iterator& it) { return it.mask_ != mask_; }

    void operator++() { update_mask(); }

    T& operator*() { return data_[idx_]; }

   private:
    iterator(unsigned long long mask, vector<T>& data)
        : idx_(0), mask_(mask), data_(data) {
      if (mask && !(mask_ & 1)) {
        update_mask();
      }
    }

    void update_mask() {
      do {
        mask_ >>= 1;
        idx_++;
      } while (mask_ && !(mask_ & 1));
    }

    int idx_;
    unsigned long long mask_;

    vector<T>& data_;
    friend class SubsetView;
  };

  iterator begin() { return iterator(mask_, data_set_); }

  iterator end() { return iterator(0, data_set_); }

  // Returns current subset
  vector<T> materialize() const {
    vector<T> result;
    int idx = 0;
    for (unsigned long long m = mask_; m; m >>= 1, idx++) {
      if (m & 1) {
        result.push_back(data_set_[idx]);
      }
    }
    return result;
  }

  size_t size() const { return 4; }

  void operator++() { mask_ = (mask_ + 1) & limit_; }

  operator bool() const { return mask_ > 0; }

  vector<T>& operator*() { return data_set_; }

  const vector<T>& operator*() const { return data_set_; }

  unsigned long long hash() { return mask_; }

  unsigned long long hash_with_el(const T& el, bool on) {
    unsigned long long idx = 0;
    for (const auto& it : data_set_) {
      if (el < it) break;
      idx++;
    }
    unsigned long long mask = mask_;
    mask >>= idx;
    mask <<= idx + 1;
    mask += (1 << idx);
    unsigned long long prefix = (1 << idx) - 1;
    prefix &= mask_;
    unsigned long long res = prefix + mask;
    if (!on) res = unset_bit(res, idx);
    return res;
  }

  unsigned long long hash_without_el(const T& el) {
    unsigned long long idx = 0;
    for (const auto& it : data_set_) {
      if (it == el) break;
      idx++;
    }
    unsigned long long mask = mask_;
    mask >>= idx + 1;
    mask <<= idx;
    unsigned long long prefix = (1 << idx) - 1;
    prefix &= mask_;
    return prefix + mask;
  }

  bool is_present(const T& el) {
    unsigned long long cp = mask_;
    int inx = -1;
    for (int i = 0; i < data_set_.size(); i++) {
      if (data_set_[i] == el) {
        inx = i;
        break;
      }
    }
    if (inx < 0) return false;
    return (cp >> inx) & 1;
  }

 private:
  unsigned long long mask_;
  unsigned long long limit_;

  vector<T>& data_set_;
};

// Class to iterate through all partitions of a given set.
template <typename T>
class PartitionView {
 public:
  PartitionView(vector<T>& data) : data_(data) {}

  class iterator {
   public:
    bool operator!=(const iterator& it) {
      if (it.last_) return !last_;
      return it.c_ != c_;
    }

    // Iterates through each partition only once.
    void operator++() {
      r_ = data_.size() - 1;
      while (c_[r_] > g_[r_ - 1] && r_) r_--;
      if (r_ == 0) {
        last_ = true;
        return;
      }
      c_[r_]++;
      g_[r_] = max(g_[r_], c_[r_]);
      r_++;
      while (r_ < data_.size()) {
        c_[r_] = 1;
        g_[r_] = g_[r_ - 1];
        r_++;
      }
    }

    vector<int>& operator*() { return c_; }

    unsigned long long hash() {
      unsigned long long result = 0;
      int base = 1;
      for (int i = 0; i < data_.size(); i++) {
        result += c_[i] * base;
        base *= data_.size();
      }
      return result;
    }

    void print() {
      printf("print c: ");
      for (const auto& it : c_) {
        printf("%d ", it);
      }
      printf("\n");
    }

    // Returns true if el is a singleton.
    bool singleton(const T& el) {
      int partition, idx;
      for (int i = 0; i < data_.size(); i++) {
        if (data_[i] == el) {
          partition = c_[i];
          idx = i;
        }
      }
      for (int i = 0; i < data_.size(); i++) {
        if (i == idx) continue;
        if (c_[i] == partition) return false;
      }
      return true;
    }

    // Returns hash of partition w/o given singleton.
    int remove_singleton(const T& el_to_remove) {
      int partition = this->partition(el_to_remove);
      unsigned long long result = 0;
      int base = 1;
      for (int i = 0; i < data_.size(); i++) {
        if (data_[i] == el_to_remove) continue;
        result += (c_[i] > partition ? c_[i] - 1 : c_[i]) * base;
        base *= data_.size() - 1;
      }
      return result;
    }

    int max_partition() { return g_[data_.size() - 1]; }

    // Returns hash for a partition modified in the way that subset part
    // contains el.
    int add_to_partition_hash(const T& el, int part) {
      int i = 0;
      while (i < data_.size() && data_[i] < el) {
        i++;
      }
      c_.insert(c_.begin() + i, part);

      unsigned long long result = 0;
      int base = 1;

      int next_part = 1;
      map<int, int> transl;
      for (int i = 0; i < data_.size() + 1; i++) {
        int c_i;
        if (transl.find(c_[i]) != transl.end()) {
          c_i = transl[c_[i]];
        } else {
          transl[c_[i]] = next_part;
          next_part++;
          c_i = transl[c_[i]];
        }

        result += c_i * base;
        base *= (data_.size() + 1);
      }
      c_.erase(c_.begin() + i);

      return result;
    }

    // Returns an index of subset that includes el.
    int partition(const T& el) const {
      for (int i = 0; i < data_.size(); i++) {
        if (data_[i] == el) return c_[i];
      }
    }

    // Merge subsets a and b.
    int merge(int a, int b) {
      int min = (a <= b ? a : b);
      int max = (a <= b ? b : a);

      unsigned long long result = 0;
      int base = 1;
      for (int i = 0; i < data_.size(); i++) {
        result +=
            (c_[i] == max ? min : (c_[i] > max ? c_[i] - 1 : c_[i])) * base;
        base *= data_.size();
      }
      return result;
    }

    // Returns map: partition subset -> all its elements
    map<int, vector<std::reference_wrapper<T>>> distribution() {
      map<int, vector<std::reference_wrapper<T>>> res;
      for (int i = 0; i < data_.size(); i++) {
        res[c_[i]].push_back(data_[i]);
      }
      return res;
    }

   private:
    iterator(vector<T>& data, bool last) : data_(data), last_(last) {
      c_ = vector<int>(data_.size(), 1);
      g_ = vector<int>(data_.size(), 1);
    }

    int r_;
    bool last_;
    vector<T> data_;
    // c_[i] = p when subset p contains i.
    vector<int> c_;
    // g_[i] = max of {c_[0],...,c_[i-1]}
    vector<int> g_;
    friend class PartitionView;
  };

  iterator begin() { return iterator(data_, false); }

  iterator end() { return iterator(data_, true); }

 private:
  vector<T>& data_;
};
