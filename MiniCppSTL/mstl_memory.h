#pragma once

#include "mstl_global.h"

NAMESPACE_MSTL

template <typename T>
class unique_ptr{
public:
    using pointer_type                =  T*;
    using reference_value_type        =  T&;

    explicit unique_ptr(pointer_type ptr = nullptr) noexcept: ptr_(ptr){}
    unique_ptr(unique_ptr<T>& other) = delete;
    unique_ptr(unique_ptr<T>&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
    ~unique_ptr() noexcept { delete ptr_; }

    unique_ptr& operator =(unique_ptr<T>& other) = delete;
    unique_ptr& operator =(unique_ptr<T>&& other) noexcept{
        if (this != &other){
            reset(other.ptr_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    pointer_type get() const noexcept                { return ptr_;}
    pointer_type operator ->() const noexcept        { return ptr_; }
    reference_value_type operator *() const noexcept { return *ptr_;}
    
    void swap(unique_ptr<T>& other) noexcept {
        pointer_type old_ptr = this->ptr_;
        this->ptr_ = other.ptr_;
        other.ptr_ = old_ptr;
    }

    void reset(pointer_type ptr = nullptr) noexcept {
        pointer_type old_ptr = ptr_;
        ptr_ = ptr;
        delete old_ptr;
    }

    pointer_type release() noexcept {
        pointer_type old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }


private:
    pointer_type ptr_;
};


END_NAMESPCE