#pragma once
#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h"
#include "mstl_iterator_base.h"

#include <type_traits>       //std::is_trivially_copyable

NAMESPACE_MSTL

/*
TODO:

*/

template <typename T, const mstl::size_t ARRAY_SIZE>
class array{
public:
    /* STL format */
    using value_type       =  T;
    using pointer          =  T*;
    using reference        =  T&;
    using const_pointer    =  const T*;
    using const_reference  =  const T&;
    using size_type        =  mstl::size_t;
    

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

    /* 简单的重载 */
    template<typename U, const mstl::size_t size>
    friend bool operator ==(const mstl::array<U, size>& lhs, const mstl::array<U, size>& rhs);
    template<typename U, const mstl::size_t size>
    friend bool operator !=(const mstl::array<U, size>& lhs, const mstl::array<U, size>& rhs);


    /* 一些类内直接定义的函数 */
    constexpr reference operator[] (size_type p)                noexcept { return data_ptr_[p]; }
    constexpr const_reference operator [] (size_type p)   const noexcept { return data_ptr_[p]; }
    constexpr reference front()                                 noexcept { return data_ptr_[0]; }
    constexpr const_reference front()                     const noexcept { return data_ptr_[0]; }
    constexpr reference back()                                  noexcept { return data_ptr_[ARRAY_SIZE - 1]; }
    constexpr const_reference back()                      const noexcept { return data_ptr_[ARRAY_SIZE - 1]; }
    constexpr size_type size()                            const noexcept { return ARRAY_SIZE; }

     /* Iterator */
    template<typename ValueType = T, typename PointerType = ValueType*, typename ReferenceType = ValueType&, const bool IS_REVERSE = false>
    class iterator_impl : public iterator_base<iterator_impl<ValueType, PointerType, ReferenceType, IS_REVERSE>, ValueType> {
    public:
        using value_type         =   ValueType;
        using pointer            =   PointerType;
        using reference          =   ReferenceType;
        using difference_type    =   std::ptrdiff_t;
        using iterator_category  =   mstl::random_access_iterator_tag;
        
        using this_type = iterator_impl<value_type, pointer, reference, IS_REVERSE>;
        
        
        iterator_impl(pointer ptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->()  { return ptr_; }

        iterator_impl& operator++() override { 
            if constexpr (IS_REVERSE){
                -- ptr_;
            }
            else{
                ++ptr_; 
            }
            return *this; 
        }
        iterator_impl operator++(int) override { 
            iterator_impl new_iter = *this; 
            if constexpr (IS_REVERSE){
                --(*this);
            }
            else{
                ++(*this); 
            }
            return new_iter; 
        }

        iterator_impl& operator--() override { 
            if constexpr (IS_REVERSE){
                ++ptr_;
            }
            else{
                --ptr_; 
            }
            return *this; 
        }
        
        iterator_impl operator--(int) override { 
            iterator_impl new_iter = *this;
            if constexpr (IS_REVERSE) {
                ++(*this);
            } else {
                --(*this);
            }
            return new_iter; 
        }

        iterator_impl operator+(difference_type offset) const { 
            if constexpr (IS_REVERSE) {
                return iterator_impl(ptr_ - offset);
            } else {
                return iterator_impl(ptr_ + offset);
            }
        }

        iterator_impl operator-(difference_type offset) const { 
            if constexpr (IS_REVERSE) {
                return iterator_impl(ptr_ + offset);
            } else {
                return iterator_impl(ptr_ - offset);
            }
        }

        difference_type operator-(const this_type& other) const { return ptr_ - other.ptr_; }
        bool operator==(const this_type& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const this_type& other) const { return !(*this == other); }
        bool operator <(const this_type& other) const { return ptr_ < other.ptr_; }
        bool operator <=(const this_type& other) const { return ptr_ <= other.ptr_; }
        bool operator >(const this_type& other) const { return !(*this <= other); }
        bool operator >=(const this_type& other) const { return !(*this < other); }

    private:
        pointer ptr_;
    };

    /* 四种迭代器类型 */
    using iterator = iterator_impl<value_type>;
    using const_iterator = iterator_impl<const value_type, const_pointer, const_reference>;
    using reverse_iterator = iterator_impl<value_type, pointer, reference, true>;
    using const_reverse_iterator = iterator_impl<const value_type, const_pointer, const_reference, true>;

    /* 正向迭代器相关函数 */
    iterator begin() { return iterator(data_ptr_); }
    iterator end()   { return iterator(data_ptr_ + ARRAY_SIZE); }
    const_iterator cbegin() { return const_iterator(data_ptr_); } 
    const_iterator cend() { return const_iterator(data_ptr_ + ARRAY_SIZE); }

    /* 逆向迭代器相关函数 */
    reverse_iterator rbegin() { return reverse_iterator(data_ptr_ + ARRAY_SIZE - 1); }
    reverse_iterator rend() { return reverse_iterator(data_ptr_ - 1); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(data_ptr_ + ARRAY_SIZE - 1); }
    const_reverse_iterator crend() { return const_reverse_iterator(data_ptr_ - 1); }

private:
    value_type data_ptr_[ARRAY_SIZE];
};

/* 检查两个array是否相等 */
template<typename T, const mstl::size_t size>
inline bool operator ==(const mstl::array<T, size>& lhs, const mstl::array<T, size>& rhs){
    if (lhs.size() != rhs.size()){
        return false;
    }
    for (decltype(lhs.size()) i = 0; i < lhs.size(); ++i){
        if (lhs[i] != rhs[i]){
            return false;
        }
    }
    return true;
}

/* 检查两个array是否不相等 */
template<typename T, const mstl::size_t size>
inline bool operator !=(const mstl::array<T, size>& lhs, const mstl::array<T, size>& rhs){
    return !(lhs == rhs);
}

END_NAMESPACE
