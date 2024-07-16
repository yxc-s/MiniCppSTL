#pragma once

#include "mstl_global.h"
#include <atomic>

NAMESPACE_MSTL

/* unique_ptr : 独占资源的指针管理对象 */
template <typename T>
class unique_ptr{
public:
    using pointer          =    T*;
    using const_pointer    =   const T*;
    using reference        =    T&;
    using const_reference  =   const T&;

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

    pointer get()       noexcept                { return ptr_; }
    const_pointer get() const noexcept          { return ptr_; }

    pointer operator ->() noexcept              { return ptr_; }
    const_pointer operator ->() const noexcept  { return ptr_; }

    reference operator *() noexcept             { return *ptr_; }
    const_reference operator *() const noexcept { return *ptr_; }

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


/* shared_ptr : 共享资源的指针管理对象 */
template<typename T>
class shared_ptr{
public:
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = mstl::size_t;

    shared_ptr(): ptr_(nullptr), cnt_(new std::atomic<size_type>(0)) {}

    shared_ptr(T* ptr) : ptr_(ptr), cnt_(new std::atomic<size_type>(ptr ? 1 : 0)) {}

    shared_ptr(const shared_ptr<T>& other) : ptr_(other.ptr_), cnt_(other.cnt_) {
        cnt_->fetch_add(1, std::memory_order_relaxed);
    }

    shared_ptr(shared_ptr<T>&& other) : ptr_(other.ptr_), cnt_(other.cnt_) {
        other.ptr_ = nullptr;
        other.cnt_ = nullptr;
    }

    ~shared_ptr() {
        if (cnt_ && cnt_->fetch_add(-1, std::memory_order_acq_rel) == 1) {
            delete ptr_;
            delete cnt_;
        }
    }

    shared_ptr& operator =(const shared_ptr<T>& other) {
        if (this != &other) {
            shared_ptr<T> temp(other);
            this->swap(temp);
        }
        return *this;
    }

    shared_ptr& operator =(shared_ptr<T>&& other) {
        if (this != &other) {
            shared_ptr<T> temp(mstl::move(other));
            this->swap(temp);
        }
        return *this;
    }

    reference operator*()                      { return *(ptr_); }
    const_reference operator*() const          { return *(ptr_); }

    pointer operator->() noexcept              { return ptr_; }
    const_pointer operator->()  const noexcept { return ptr_; }

    pointer get() noexcept                     { return ptr_; }
    const_pointer get() const noexcept         { return ptr_; }

    size_type use_count() const noexcept       { return cnt_ ? cnt_->load(std::memory_order_relaxed) : 0; }

    void swap(shared_ptr<T>& other) {
        mstl::swap(ptr_, other.ptr_);
        mstl::swap(cnt_, other.cnt_);
    }

    void reset() noexcept { shared_ptr<T>().swap(*this); }
    void reset(pointer ptr) { shared_ptr<T>(ptr).swap(*this); }


private:
    pointer ptr_;
    std::atomic<size_type>* cnt_;
};


END_NAMESPACE