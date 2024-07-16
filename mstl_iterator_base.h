#pragma once

#include "mstl_global.h"

#include <cstddef>  // for std::ptrdiff_t

NAMESPACE_MSTL


//TODO: 删除纯虚函数接口后，要在随机访问容器里改一下override签名 
/* 奇异递归模板模式(Curiously Recurring Template Pattern) */
template <typename Derived, typename U>
class iterator_base {
public:
    using difference_type  =  std::ptrdiff_t;

    virtual ~iterator_base() = default;


    virtual Derived& operator++() = 0;
    virtual Derived operator++(int) = 0;

    // virtual Derived& operator--() {};
    // virtual Derived operator--(int){};

    // virtual Derived operator+(difference_type offset) const {};
    // virtual Derived operator-(difference_type offset) const {};

    // virtual difference_type operator-(const Derived& other) const {};

    virtual bool operator==(const Derived& other) const = 0;
    virtual bool operator!=(const Derived& other) const = 0;
};


END_NAMESPACE