#pragma once
#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h"
#include "mstl_iterator_base.h"

#include <type_traits>

NAMESPACE_MSTL

/*
TODO:
    initialize_list
    iterator
*/

template <typename T, const mstl::size_t ARRAY_SIZE>
class array{
public:
    using value_type                  =  T;
    using pointer_type                =  T*;
    using reference_value_type        =  T&;
    using const_reference_value_type  =  const T&;
    using size_type                   =  mstl::size_t;
    

    constexpr array() noexcept: data_ptr_{}{
        if constexpr (!std::is_trivially_copyable<T>::value) {
            for (size_type i = 0; i < ARRAY_SIZE; ++i){
                new(&data_ptr_[i]) T();
            }
        }
    }

    constexpr array(const std::initializer_list<T>& initializer) noexcept : data_ptr_{}{
        size_type p = 0;
        for (const auto& value : initializer){
            if (p < ARRAY_SIZE){
                data_ptr_[p ++] = value;
            }
            else{
                return;
            }
        }
    }

    ~array() = default;


    constexpr void fill(const T& value) noexcept{
        for (size_type i = 0; i < ARRAY_SIZE; ++i){
            data_ptr_[i] = value;
        }
    }

    constexpr reference_value_type operator[] (size_type p)                noexcept { return data_ptr_[p]; }
    constexpr const_reference_value_type operator [] (size_type p)   const noexcept { return data_ptr_[p]; }

    constexpr reference_value_type front()                                 noexcept { return data_ptr_[0]; }
    constexpr const_reference_value_type front()                     const noexcept { return data_ptr_[0]; }

    constexpr reference_value_type back()                                  noexcept { return data_ptr_[ARRAY_SIZE - 1]; }
    constexpr const_reference_value_type back()                      const noexcept { return data_ptr_[ARRAY_SIZE - 1]; }

    constexpr size_type size() const noexcept { return ARRAY_SIZE; }

    /* Iterator */
    template<typename ValueType = T, typename PointerType = ValueType*, typename ReferenceType = ValueType&>
    class iterator_impl : public iterator_base<iterator_impl<ValueType>, ValueType> {
    public:
        using iter_pointer_type     =   PointerType;
        using iter_reference_type   =   ReferenceType;
        using iter_difference_type  =   std::ptrdiff_t;

        iterator_impl(iter_pointer_type ptr) : ptr_(ptr) {}

        /* 非虚函数， type根据容器来指定 */
        iter_reference_type operator*() const { return *ptr_; }
        iter_pointer_type operator->()  { return ptr_; }

        iterator_impl& operator++() override { ++ptr_; return *this; }
        iterator_impl operator++(int) override { iterator_impl new_iter = *this; ++(*this); return new_iter; }

        iterator_impl& operator--() override { --ptr_; return *this; }
        iterator_impl operator--(int) override { iterator_impl new_iter = *this; --(*this); return new_iter; }

        iterator_impl operator+(iter_difference_type offset) const override { return iterator_impl(ptr_ + offset); }
        iterator_impl operator-(iter_difference_type offset) const override { return iterator_impl(ptr_ - offset); }

        iter_difference_type operator-(const iterator_base<iterator_impl, ValueType>& other) const override { 
            const iterator_impl& other_iter = static_cast<const iterator_impl&>(other);
            return ptr_ - other_iter.ptr_; 
        }

        bool operator==(const iterator_base<iterator_impl, ValueType>& other) const override { 
            const iterator_impl& other_iter = static_cast<const iterator_impl&>(other);
            return ptr_ == other_iter.ptr_; 
        }

        bool operator!=(const iterator_base<iterator_impl, ValueType>& other) const override { 
            const iterator_impl& other_iter = static_cast<const iterator_impl&>(other);
            return ptr_ != other_iter.ptr_; 
        }
        
    private:
        iter_pointer_type ptr_;
    };

    using iterator = iterator_impl<T>;
    using const_iterator = iterator_impl<const T, const T*, const T&>;

    iterator begin() { return iterator(data_ptr_); }
    iterator end()   { return iterator(data_ptr_ + ARRAY_SIZE); }
    const_iterator cbegin() { return const_iterator(data_ptr_); } 
    const_iterator cend() { return const_iterator(data_ptr_ + ARRAY_SIZE); }


private:
    value_type data_ptr_[ARRAY_SIZE];
};

END_NAMESPACE