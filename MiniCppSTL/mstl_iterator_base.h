#pragma once

#include "mstl_global.h"
#include <cstddef>  // for std::ptrdiff_t

NAMESPACE_MSTL

/* 奇异递归模板模式(Curiously Recurring Template Pattern) */
template <typename Derived, typename U>
class iterator_base {
public:
    using iter_difference_type  =  std::ptrdiff_t;

    virtual ~iterator_base() = default;


    virtual Derived& operator++() = 0;
    virtual Derived operator++(int) = 0;

    virtual Derived& operator--() = 0;
    virtual Derived operator--(int) = 0;

    virtual Derived operator+(iter_difference_type offset) const = 0;
    virtual Derived operator-(iter_difference_type offset) const = 0;

    virtual iter_difference_type operator-(const Derived& other) const = 0;

    virtual bool operator==(const Derived& other) const = 0;
    virtual bool operator!=(const Derived& other) const = 0;
};


/* next iterator */
template <typename ITER_TYPE>
inline ITER_TYPE next(const ITER_TYPE& iter){
    return iter + 1;
}

/* previous iterator */
template <typename ITER_TYPE>
inline ITER_TYPE prev(const ITER_TYPE& iter){
    return iter - 1;
}

END_NAMESPACE