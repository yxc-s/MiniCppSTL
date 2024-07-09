#pragma once
#include "mstl_global.h"
#include <limits>            //std::numeric_limits

NAMESPACE_MSTL

template <class T>
class Allocator {
public:
    using size_type          =   mstl::size_t;
    using value_type         =   T;
    using pointer            =   T*;
    using const_pointer      =   const T*;
    using difference_type    =   std::ptrdiff_t;

    
    Allocator() noexcept {}
    Allocator(const Allocator&) noexcept {}
    template <class U>
    Allocator(const Allocator<U>&) noexcept {}
    ~Allocator() noexcept {}

    pointer allocate(size_type size, const void* hint = 0);
    void deallocate(pointer p, size_type size);

    template <class U>
    struct rebind {
        using other = Allocator<U>;
    };

    // Return address of values
    pointer address(value_type& value) const { return &value; }
    const_pointeraddress(const value_type& value) const { return &value; }
    
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(value_type); }
};


template<typename T>
inline typename Allocator<T>::pointer Allocator<T>::allocate(size_type size, const void* hint) {
    return static_cast<pointer>(::operator new(size * sizeof(T)));
}

template<typename T>
inline void Allocator<T>::deallocate(pointer p, size_type size){
    ::operator delete((void*) p); 
}


END_NAMESPACE