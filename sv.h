#ifndef SPARSE_VECTOR_H___
#define SPARSE_VECTOR_H___
#include <cassert>
#include <type_traits>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>
#include <functional>

namespace sprs {
using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

template<typename T, typename size_type=u32, typename=std::enable_if_t<std::is_arithmetic_v<T>>, typename=std::enable_if_t<std::is_integral_v<size_type>>>
class SparseVector {
    size_type     l_; // Length of vector
    size_type     w_; // Number of non-zeros (weight)
    T            *v_; // Values
    size_type    *i_; // Indices
    bool is_sorted_;
public:
    SparseVector(size_t n, size_t reserve_size=0):
        l_(n),
        w_(0),
        v_(reserve_size ? static_cast<T *>(std::malloc(reserve_size * sizeof(T))): nullptr),
        i_(reserve_size ? static_cast<size_type *>(std::malloc(reserve_size * sizeof(size_type))): nullptr),
        is_sorted_(true)
    {
        if(reserve_size && (!v_ || !i_)) throw std::bad_alloc();
    }
    struct iterator {
        SparseVector  &ref_;
        size_type      ind_;
        using difference_type = std::int64_t;
        using DerefType = std::pair<std::reference_wrapper<size_type>, std::reference_wrapper<T>>;
        iterator(SparseVector &ref, size_type ind): ref_(ref), ind_(ind) {}
        DerefType operator*() {
            return DerefType(ref_.i_[ind_], ref_.v_[ind_]);
        }
        iterator operator++(int) {
            iterator ret(ref_, ind_);
            ++ind_;
            return ret;
        }
        iterator &operator++() {
            ++ind_;
            return *this;
        }
        bool operator<(const iterator &other) const {
            assert(&ref_ == &other.ref_);
            return ind_ < other.ind_;
        }
        bool operator<=(const iterator &other) const {
            assert(&ref_ == &other.ref_);
            return ind_ <= other.ind_;
        }
        bool operator>(const iterator &other) const {
            assert(&ref_ == &other.ref_);
            return ind_ > other.ind_;
        }
        bool operator>=(const iterator &other) const {
            assert(&ref_ == &other.ref_);
            return ind_ >= other.ind_;
        }
        bool operator==(const iterator &other) const {return other.ind_ == ind_;}
        bool operator!=(const iterator &other) const {return other.ind_ != ind_;}
    };
    void sort() {
        std::sort(begin(), end());
        is_sorted_ = true;
    }
    void double_size() {
        const auto dsz = w_ ? w_ << 1: 4;
        {
            auto tmp = static_cast<T *>(std::realloc(v_, sizeof(T) * dsz));
            if(__builtin_expect(!tmp, 0)) throw std::bad_alloc();
            v_ = tmp;
        }
        auto tmp = static_cast<size_type *>(std::realloc(i_, sizeof(size_type) * dsz));
        if(__builtin_expect(!tmp, 0)) throw std::bad_alloc();
        i_ = tmp;
    }
    template<typename... Args>
    void emplace_back(size_type index, Args &&... args) {
        if(__builtin_expect(w_ == l_, 0)) double_size();
        i_[w_] = index;
        new (v_ + w_++) T(std::forward<Args>(args)...);
    }
    ~SparseVector() {
        if(!std::is_trivially_destructible_v<T>) std::for_each(v_, v_ + w_, [](T &el){el.~T();});
    }
    iterator begin() {return iterator(*this, 0);}
    iterator end()   {return iterator(*this, w_);}
    // TODO: Add insertion of elements and lookup
    size_t size() const {return l_;}
    size_t weight() const {return w_;}
};

#if 0
class ConnectionCountSequence: public AbstractAccumulator {
    std::vector<u16> nconnections_vector_; // Number of connections
    std::vector<u32> dconnections_vector_; // Duration of connections
    
};
#endif

} /* namespace sprs */
#endif /* #ifndef SPARSE_VECTOR_H___ */
