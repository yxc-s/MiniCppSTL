#pragma once
#include "mstl_global.h"
#include <limits>

NAMESPACE_MSTL

template <class T>
class Allocator {
public:
    using size_type          =   mstl::size_t;
    using value_type         =   T;
    using pointer_type       =   T*;
    using const_pointer_type =   const T*;
    using difference_type    =   std::ptrdiff_t;

    
    Allocator() noexcept {}
    Allocator(const Allocator&) noexcept {}
    template <class U>
    Allocator(const Allocator<U>&) noexcept {}
    ~Allocator() noexcept {}

    pointer_type allocate(size_type size, const void* hint = 0);
    void deallocate(pointer_type p, size_type size);

    template <class U>
    struct rebind {
        using other = Allocator<U>;
    };

    // Return address of values
    pointer_type address(value_type& value) const { return &value; }
    const_pointer_type address(const value_type& value) const { return &value; }
    
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(value_type); }
};


template<typename T>
inline typename Allocator<T>::pointer_type Allocator<T>::allocate(size_type size, const void* hint) {
    return static_cast<pointer_type>(::operator new(size * sizeof(T)));
}

template<typename T>
inline void Allocator<T>::deallocate(pointer_type p, size_type size){
    ::operator delete((void*) p); 
}


END_NAMESPACE