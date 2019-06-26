#ifndef ALGORITHM_DISJOINT_SET_H_
#define ALGORITHM_DISJOINT_SET_H_

#include <functional>
#include <unordered_map>
#include <list>
#include <vector>

template <typename T>
class DisjointSet {
    public:

#define $_(_mem_loc_) (*(reinterpret_cast<InternalSetRepresentation*>(data_[_mem_loc_])))

    class Representative {
        friend class DisjointSet;
        public:
        Representative(const T& t) : repr_(t) {}
        bool operator==(const Representative& a) {
            return a.repr_ == repr_;
        }

        private:
        const T& repr_;
    };

    Representative find(const T& t) {
        std::list<std::reference_wrapper<const T>> path;
        path.push_back(t);
        while (!IsRepresentative(path.back().get())) {
            path.push_back(GetObjectRepresentative(path.back()));
        }
        RelaxRepresentativePath(path.back(), path);
        return Representative(path.back());
    }

    template <typename It>
        static DisjointSet<T> Create(const It& begin, const It& end) {
            DisjointSet<T> set;
            for (It it = begin; it != end; ++it) {
                set.add(*it);
            }
            return set;
        }

    bool join(const T& a, const T& b) {
        auto repr_a = find(a);
        auto repr_b = find(b);
        if (repr_a == repr_b) {
            return false;
        }

        const auto& internal_a = repr_a.repr_; // GetObjectRepresentative(repr_a);
        const auto& internal_b = repr_b.repr_; // GetObjectRepresentative(repr_b);
        if ($_(internal_a).set_rank_ > $_(internal_b).set_rank_) {
            LinkInternalSetRepresentation(internal_b, internal_a);
        } else {
            LinkInternalSetRepresentation(internal_a, internal_b);
        }
        return true;
    }

    void add(const T& t) {
        if (data_.find(t) == data_.end()) {
            new (&data_[t]) InternalSetRepresentation(t);
        }
    }

    private:

    struct InternalSetRepresentation {
        InternalSetRepresentation(const T& t, const size_t size = 1) : representative_(t), set_rank_(size) {} 
        size_t set_rank_;
        std::reference_wrapper<const T> representative_;
    };
    
    void LinkInternalSetRepresentation(const T& ta, const T& tb) {
        new (&data_[tb]) InternalSetRepresentation(ta, 1 + $_(tb).set_rank_);
    }

    typedef char internal_repr_mem[sizeof(InternalSetRepresentation)];

    bool IsRepresentative(const T& t) {
        return GetObjectRepresentative(t) == t;
    }

    const T& GetObjectRepresentative(const T& t) {
        return $_(t).representative_.get();
    }

    void RelaxRepresentativePath(const T& representative,
            const std::list<std::reference_wrapper<const T>>& repr_path) {
        const InternalSetRepresentation& root_repr = $_(representative);
        for (auto obj : repr_path) {
            new (&data_[obj]) InternalSetRepresentation(representative, root_repr.set_rank_);
        }
    }


    std::unordered_map<T, internal_repr_mem> data_;
};

#endif  // ALGORITHM_DISJOINT_SET_H_
