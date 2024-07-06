#pragma once
#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h"


#include <type_traits>

NAMESPACE_MSTL

/*
TODO:
    initialize_list
    iterator
*/

template <typename T, const size_t ARRAY_SIZE>
class array{
public:
    using value_type                  =  T;
    using pointer_type                =  T*;
    using reference_value_type        =  T&;
    using const_reference_value_type  =  const T&;
    using size_type                   =  size_t;
    

    constexpr array() noexcept: data_ptr_{}{
        if constexpr (!std::is_trivially_copyable<T>::value) {
            for (size_type i = 0; i < ARRAY_SIZE; ++i){
                new(&data_ptr_[i]) T();
            }
        }
    }

    ~array() = default;


    constexpr void fill(const T& value) noexcept{
        for (size_type i = 0; i < ARRAY_SIZE; ++i){
            data_ptr_[i] = value;
        }
    }

    constexpr reference_value_type operator[] (size_t p)                noexcept { return data_ptr_[p]; }
    constexpr const_reference_value_type operator [] (size_t p)   const noexcept { return data_ptr_[p]; }

    constexpr reference_value_type front()                              noexcept { return data_ptr_[0]; }
    constexpr const_reference_value_type front()                  const noexcept { return data_ptr_[0]; }

    constexpr reference_value_type back()                               noexcept { return data_ptr_[ARRAY_SIZE - 1]; }
    constexpr const_reference_value_type back()                   const noexcept { return data_ptr_[ARRAY_SIZE - 1]; }

    constexpr size_type size() const noexcept { return ARRAY_SIZE; }

private:
    value_type data_ptr_[ARRAY_SIZE];
};

END_NAMESPCE
