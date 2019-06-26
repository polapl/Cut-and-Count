#pragma once

#include <algorithm>
#include <vector>
#include <map>
using namespace std;

template <typename C, typename A>
bool contains(const C& container, const A& value) {
    return container.find(value) != container.end();
}

template <typename A>
bool contains(const vector<A>& container, const A& value) {
    return find(container.begin(), container.end(), value) != container.end();
}

template <typename T>
class SubsetView {
    public:
        SubsetView(vector<T>& data) :
            mask_(1),
            limit_((static_cast<unsigned long long>(1) << data.size()) -1),
            data_set_(data) {
            }

        class iterator {
            public:
                bool operator==(const iterator& it) {
                    return it.mask_ == mask_;
                }
                bool operator!=(const iterator& it) {
                    return it.mask_ != mask_;
                }

                void operator++() {
                    update_mask();
                }

                T& operator*() {
                    return data_[idx_];
                }

            private:
                iterator(unsigned long long mask, vector<T>& data) : idx_(0), mask_(mask), data_(data) {
                    if (mask && !(mask_ & 1)) {
                        update_mask();
                    }
                }

                void update_mask() {
                    do {
                        mask_ >>=1;
                        idx_++;
                    } while  (mask_ && !(mask_ & 1)); 
                }
                
                int idx_;
                unsigned long long mask_;

                vector<T>& data_;
                friend class SubsetView;
        };

        iterator begin() {
            return iterator(mask_, data_set_);
        }

        iterator end() {
            return iterator(0, data_set_);
        }
        vector<T> materialize() const {
            vector<T> result;
            int idx = 0;
            for (unsigned long long m = mask_; m; m>>=1, idx++) {
                if  (m & 1) {
                    result.push_back(data_set_[idx]);
                }
            }
            return result;
        }

        size_t size() const {
            return 4;
        }

        void operator++() {
            mask_ = (mask_+1) & limit_; 
        }

        operator bool() const {
            return mask_ > 0;
        }

        vector<T>& operator*() {
            return data_set_;
        }

        const vector<T>& operator*() const {
            return data_set_;
        }

        unsigned long long hash() {
            return mask_;
        }

        bool is_present(T el) {
            unsigned long long cp = mask_;
            int inx = -1;
            for (int i=0; i<data_set_.size(); i++) {
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

template <typename T>
class PartitionView {
    public:
        PartitionView(vector<T>& data): data_(data) {}

        class iterator {
            public:
                /*bool operator==(const iterator& it) {
                    if (it.r_ == 1) return r_ == 1;
                    return c_ == it.c_;
                }*/

                bool operator!=(const iterator& it) {
                    printf("op: %d %d\n", it.r_, r_);
                    if (it.r_ != r_) return true;
                    if (r_ == 1) return false;
                    return it.c_ != c_;
                }

                void operator++() {
                    while (c_[r_] > g_[r_-1] && r_) {
                        r_--;
                    }
                    if (r_ == 1) return;
                    if (r_) c_[r_] = c_[r_]+1;
                    if (c_[r_] > g_[r_]) {
                        g_[r_] = c_[r_];
                    }
                    while (r_ < n1_) {
                        r_++;
                        c_[r_] = 1;
                        g_[r_] = g_[r_-1];
                    }
                    for (int j = 1; j <= g_[n1_] + 1; ++j) {
                        c_[n1_] = j;
                        print();
                    }
                    if (n1_) r_ = 1;
                }

                vector<int>& operator*() {
                    return c_;
                }

                unsigned long long hash() {
                    unsigned long long result = 0;
                    int base = 1; 
                    for (int i=1; i<data_.size(); i++) {
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
                    printf("print g: ");
                    for (const auto& it : g_) {
                        printf("%d ", it);
                    }
                    printf("\nr_=%d\n", r_);
                }

                bool singleton(T el) {
                    int partition;
                    for (int i=1; i < data_.size(); i++) {
                        if (data_[i] == el) partition = c_[i];
                    }
                    for (int i=1; i < data_.size(); i++) {
                        if (c_[i] == partition) return false;
                    }
                    return true;
                }

                void remove_singleton(T& el_to_remove) {
                    int partition = -1, idx;
                    for (int i=1; i < data_.size(); i++) {
                        if (partition >= 0) c_[i]--;
                        g_[i] = max(g_[i-1], c_[i]);
                        if (data_[i] != el_to_remove) continue;
                        partition = c_[i];
                        idx = i;
                    }
                    data_.erase(data_.begin() + idx);
                    c_.erase(c_.begin() + idx);
                    g_.erase(g_.begin() + idx);
                    n1_ = data_.size()-1;
                }

            private:
                iterator(vector<T>& data, int r) : data_(data), r_(r) {
                    data_.insert(data_.begin(), T());
                    c_ = vector<int>(data_.size());
                    g_ = vector<int>(data_.size());
                    c_[0] = 0;
                    n1_ = data_.size()-1;
                    g_[0] = 0;
                }

                int r_, n1_;
                vector<T> data_;
                vector<int> c_;
                vector<int> g_;
                friend class PartitionView;
        };

        iterator begin() {
            return iterator(data_, 0);
        }

        iterator end() {
            return iterator(data_, 1);
        }

    private:
        vector<T>& data_;
};

/*
 Sample usage:
 int main() {
    vector<string> A{"Pola","Ala","Kasia","Basia","Wania", "Andrzejos"};
    SubsetView<string> sset(A);
    for (; sset; ++sset) {
        cout << "{ ";
        for (const auto& o : sset) {
            cout << " " << o;
        }
        cout << " }\n";
    } 
    return 0;
}

*/
