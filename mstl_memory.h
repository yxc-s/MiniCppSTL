#pragma once

#include "mstl_global.h"

NAMESPACE_MSTL

template <typename T>
class unique_ptr{
public:
    using pointer       =  T*;
    using reference     =  T&;

    explicit unique_ptr(pointer ptr = nullptr) noexcept: ptr_(ptr){}
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

    pointer get() const noexcept                { return ptr_;}
    pointer operator ->() const noexcept        { return ptr_; }
    reference operator *() const noexcept { return *ptr_;}
    
    void swap(unique_ptr<T>& other) noexcept {
        pointer old_ptr = this->ptr_;
        this->ptr_ = other.ptr_;
        other.ptr_ = old_ptr;
    }

    void reset(pointer ptr = nullptr) noexcept {
        pointer old_ptr = ptr_;
        ptr_ = ptr;
        delete old_ptr;
    }

    pointer release() noexcept {
        pointer old_ptr = ptr_;
        ptr_ = nullptr;
        return old_ptr;
    }


private:
    pointer ptr_;
};


END_NAMESPACE