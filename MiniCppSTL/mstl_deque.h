#pragma once

#include "mstl_global.h"
#include "mstl_allocator.h"

#include <type_traits>

NAMESPACE_MSTL

template<typename T, typename Allocator = mstl::Allocator<T>>
class deque{
public:
    /* STL format */
    using value_type       =  T;
    using pointer          =  T*;
    using reference        =  T&;
    using const_pointer    =  const T*;
    using const_reference  =  const T&;
    using size_type        =  mstl::size_t;

    explicit deque(size_type size = 0);
    deque(size_type size, const value_type& value);
    deque(const deque<value_type>& other);
    deque(deque<value_type>&& other);
    deque(const std::initializer_list<value_type>& initializer) noexcept;
    /* 迭代器构造 */
    template<typename InputIterator, typename = typename std::enable_if<
        std::is_same_v<typename InputIterator::value_type, value_type>>::type>
    deque(const InputIterator& begin, const InputIterator& end);
    ~deque();

    void push_front(const T& value);
    void push_front(T&& value);
    void push_back(const T& value);
    void push_back(T&& value);

    void pop_back();

    void clear();

    void reserve();

    /* iterator*/

    

private:



};


END_NAMESPACE