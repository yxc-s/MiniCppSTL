#pragma once
#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h"

#include <cstring>    //memset, memcpy
#include <type_traits>
#include <utility>


NAMESPACE_MSTL

/*
TODO:
    emplace_back
    iterator
*/
template<typename T, typename Allocator = mstl::Allocator<T>>
class vector{
public:
    using value_type                  =  T;
    using pointer_type                =  T*;
    using reference_value_type        =  T&;
    using const_reference_value_type  =  const T&;
    using size_type                   =  size_t;
    
    explicit vector(size_type size = 0);
    vector(size_type size, const value_type& value);
    vector(const vector<value_type>& other);
    vector(vector<value_type>&& other) noexcept;
    ~vector();

    
    void push_back(const T& value);

    void pop_back();
    
    void reserve(size_t n);

    void fill(const T& value);

    vector<value_type, Allocator>& operator =(const vector<value_type>& other);
    vector<value_type, Allocator>& operator =(vector<value_type>&& other) noexcept;

    template<typename U>
    friend bool operator ==(const mstl::vector<U>& lhs, const mstl::vector<U>& rhs);

    reference_value_type operator[] (size_t p)                { return data_ptr_[p]; }
    const_reference_value_type operator [] (size_t p)   const { return data_ptr_[p]; }

    reference_value_type front()                              { return data_ptr_[0]; }
    const_reference_value_type front()                  const { return data_ptr_[0]; }

    reference_value_type back()                               { return data_ptr_[cur_size_ - 1]; }
    const_reference_value_type back()                   const { return data_ptr_[cur_size_ - 1]; }

    size_type size()                           const noexcept { return cur_size_; }
    
private:
    mstl::unique_ptr<Allocator>        allocator_;
    pointer_type                       data_ptr_;
    size_type                          cur_size_;
    size_type                          max_size_;
    
    void releaseMemory() {
        for (size_type i = 0; i < cur_size_; ++i){
            data_ptr_[i].~T();
        }
    }
};


template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type size): 
    allocator_(new Allocator{}),
    data_ptr_(allocator_->allocate(size)),
    cur_size_(size),
    max_size_(size)
{
    if (max_size_ > 0){
        for (size_type i = 0; i < cur_size_; ++i) {
            new(data_ptr_ + i) T();
        }
    }
}

template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type size, const value_type& value): 
    allocator_(new Allocator{}),
    data_ptr_(allocator_->allocate(size)),
    cur_size_(size),
    max_size_(size)
{
    if (max_size_ > 0){
        for (size_type i = 0; i < cur_size_; ++i) {
            new(data_ptr_ + i) T(value);
        }
    }
}

template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(const vector<T>& other):
    allocator_(new Allocator),
    cur_size_(other.cur_size_),
    max_size_(other.max_size_),
    data_ptr_(allocator_->allocate(max_size_))
{   
    if constexpr (std::is_trivially_copyable<T>::value) {
        memcpy(data_ptr_, other.data_ptr_, sizeof(T) * cur_size_);
    }
    else{
        for (size_type i = 0; i < cur_size_; ++i) {
            new(data_ptr_ + i) T(other.data_ptr_[i]);
        }
    }    
}

template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(vector<T>&& other) noexcept:
    allocator_(new Allocator),
    data_ptr_(other.data_ptr_),
    max_size_(other.max_size_),
    cur_size_(other.cur_size_)
{   
    other.max_size_ = 0;
    other.cur_size_ = 0;
    other.data_ptr_ = nullptr;
}


template<typename T, typename Allocator>
inline vector<T, Allocator>::~vector(){
    if (data_ptr_){
        releaseMemory();
        allocator_->deallocate(data_ptr_, max_size_);
    }
    data_ptr_ = nullptr;
}

template<typename T, typename Allocator>
inline void vector<T, Allocator>::reserve(size_type size){
    if (size > max_size_){
        pointer_type new_pointer = allocator_->allocate(size);
        if constexpr (std::is_trivially_copyable<T>::value) {
            memcpy(new_pointer, data_ptr_, sizeof(T) * cur_size_);
        }
        else{
            for (size_type i = 0; i < cur_size_; ++i) {
                /* Placement New (new (address) T(constructor_args)) */
                new(new_pointer + i) T(std::move(data_ptr_[i]));
                data_ptr_[i].~T();
            }
        }
        releaseMemory();
        allocator_->deallocate(data_ptr_, max_size_);
        max_size_ = size;
        data_ptr_ = new_pointer;
    }
}

template<typename T, typename Allocator>
inline void vector<T, Allocator>::fill(const T& value){
    for (size_type i = 0; i < cur_size_; ++i){
        data_ptr_[i] = value;
    }
}

template<typename T, typename Allocator>
inline vector<T, Allocator>& vector<T, Allocator>::operator =(const vector<T>& other){
    if (this != &other){
        if (data_ptr_){
            releaseMemory();
        }
        if (max_size_ < other.cur_size_){
            if (data_ptr_){
                allocator_->deallocate(data_ptr_, max_size_);   
            }
            data_ptr_ = allocator_->allocate(other.max_size_);
            max_size_ = other.max_size_;
        }
        if constexpr (std::is_trivially_copyable<value_type>::value) {
            memcpy(data_ptr_, other.data_ptr_, sizeof(value_type) * other.cur_size_);
        }
        else{
            for (size_type i = 0; i < cur_size_; ++i) {
                /* Placement New (new (address) T(constructor_args)) */
                new(data_ptr_ + i) T(other.data_ptr_[i]);
            }
        }
        cur_size_ = other.cur_size_;
    }
    return *this;
}

template<typename T, typename Allocator>
inline vector<T, Allocator>& vector<T, Allocator>::operator =(vector<T>&& other) noexcept{
    if (this != &other){
        if (data_ptr_){
            releaseMemory();
            allocator_->deallocate(data_ptr_, max_size_);
        }
        data_ptr_ = other.data_ptr_;
        cur_size_ = other.cur_size_;
        max_size_ = other.max_size_;
        other.data_ptr_ = nullptr;
        other.max_size_ = 0;
        other.cur_size_ = 0;
    }
    return *this;
}

template<typename U>
inline bool operator ==(const mstl::vector<U>& lhs, const mstl::vector<U>& rhs){
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

template<typename T, typename Allocator>
inline void vector<T, Allocator>::push_back(const T& value){
    if (cur_size_ == max_size_){
        this->reserve((max_size_ > 0 ? (max_size_ << 1) : 2));
    }
    new (data_ptr_ + cur_size_) T(value);
    ++cur_size_;
}

template<typename T, typename Allocator>
inline void vector<T, Allocator>::pop_back(){
    if (cur_size_ > 0) {
        cur_size_ --;
        if constexpr (!std::is_trivially_destructible<T>::value) {
            data_ptr_[cur_size_].~T();
        }
    }
}

END_NAMESPCE