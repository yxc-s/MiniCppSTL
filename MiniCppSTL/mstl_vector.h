#pragma once
#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h"
#include "mstl_iterator_base.h"
#include "mstl_utility.h"    


#include <cstring>           //memset, memcpy
#include <type_traits>       //std::is_trivially_copyable


NAMESPACE_MSTL

/*
TODO:
    emplace_back
    erase

    重载运算符
*/
template<typename T, typename Allocator = mstl::Allocator<T>>
class vector{
public:
    /* STL format */
    using value_type       =  T;
    using pointer          =  T*;
    using reference        =  T&;
    using const_pointer    =  const T*;
    using const_reference  =  const T&;
    using size_type        =  mstl::size_t;

    explicit vector(size_type size = 0);
    vector(size_type size, const value_type& value);
    vector(const vector<value_type>& other);
    vector(vector<value_type>&& other) noexcept;
    vector(const std::initializer_list<value_type>& initializer) noexcept;
    /* 迭代器构造 */
    template<typename InputIterator, typename = typename std::enable_if<
        std::is_same_v<typename InputIterator::value_type, value_type>>::type>
    vector(const InputIterator& begin, const InputIterator& end);
    ~vector();


    void push_back(const value_type& value);
    void push_back(value_type&& value);

    void pop_back();
    
    void reserve(size_type n);

    /* TODO: 弃用这个函数? */
    void fill(const value_type& value);

    void clear() noexcept;

    vector<value_type, Allocator>& operator =(const vector<value_type>& other);
    vector<value_type, Allocator>& operator =(vector<value_type>&& other) noexcept;

    template<typename U>
    friend bool operator ==(const mstl::vector<U>& lhs, const mstl::vector<U>& rhs);

    reference operator[] (size_type p)              { return data_ptr_[p]; }
    const_reference operator [] (size_type p) const { return data_ptr_[p]; }

    reference front()                               { return data_ptr_[0]; }
    const_reference front()                   const { return data_ptr_[0]; }

    reference back()                                { return data_ptr_[cur_size_ - 1]; }
    const_reference back()                    const { return data_ptr_[cur_size_ - 1]; }

    size_type size()                            const noexcept { return cur_size_; }
    size_type capacity()                        const noexcept { return max_size_; }
    bool empty()                                const noexcept { return static_cast<bool>(cur_size_ > 0); }

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

        /* 派生类函数， type根据模板参数来指定 */
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

        iterator_impl operator+(difference_type offset) const override { 
            if constexpr (IS_REVERSE) {
                return iterator_impl(ptr_ - offset);
            } else {
                return iterator_impl(ptr_ + offset);
            }
        }

        iterator_impl operator-(difference_type offset) const override { 
            if constexpr (IS_REVERSE) {
                return iterator_impl(ptr_ + offset);
            } else {
                return iterator_impl(ptr_ - offset);
            }
        }

        difference_type operator-(const this_type& other) const override { 
            return ptr_ - other.ptr_; 
        }

        bool operator==(const this_type& other) const override { 
            return ptr_ == other.ptr_; 
        }

        bool operator!=(const this_type& other) const override { 
            return ptr_ != other.ptr_; 
        }

        bool operator <(const this_type& other) const{
            return ptr_ < other.ptr_;
        }
        
        bool operator <=(const this_type& other) const{
            return ptr_ <= other.ptr_;
        }

        bool operator >(const this_type& other) const{
            return !(*this <= other);
        }

        bool operator >=(const this_type& other) const{
            return !(*this < other);
        }

    private:
        pointer ptr_;
    };

    using iterator = iterator_impl<value_type>;
    using const_iterator = iterator_impl<const value_type, const_pointer, const_reference>;
    using reverse_iterator = iterator_impl<value_type, pointer, reference, true>;
    using const_reverse_iterator = iterator_impl<const value_type, const_pointer, const_reference, true>;

    iterator begin() { return iterator(data_ptr_); }
    iterator end()   { return iterator(data_ptr_ + cur_size_); }
    const_iterator cbegin() { return const_iterator(data_ptr_); } 
    const_iterator cend() { return const_iterator(data_ptr_ + cur_size_); }
    
    reverse_iterator rbegin() { return reverse_iterator(data_ptr_ + cur_size_ - 1); }
    reverse_iterator rend() { return reverse_iterator(data_ptr_ - 1); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(data_ptr_ + cur_size_ - 1); }
    const_reverse_iterator crend() { return const_reverse_iterator(data_ptr_ - 1); }

private:
    Allocator                          allocator_;
    pointer                            data_ptr_;
    size_type                          cur_size_;
    size_type                          max_size_;
    
    void releaseMemory() {
        for (size_type i = 0; i < cur_size_; ++i){
            data_ptr_[i].~T();
        }
    }
};

/* Placement new 表达式: new (address) Type(arguments) */

template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type size): 
    data_ptr_(allocator_.allocate(size)),
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
    data_ptr_(allocator_.allocate(size)),
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
    cur_size_(other.cur_size_),
    max_size_(other.max_size_),
    data_ptr_(allocator_.allocate(max_size_))
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
    data_ptr_(other.data_ptr_),
    max_size_(other.max_size_),
    cur_size_(other.cur_size_)
{   
    other.max_size_ = 0;
    other.cur_size_ = 0;
    other.data_ptr_ = nullptr;
}

template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(const std::initializer_list<T>& initializer) noexcept:
    data_ptr_(allocator_.allocate(static_cast<size_type>(initializer.size()))),
    max_size_(static_cast<size_type>(initializer.size())),
    cur_size_(static_cast<size_type>(initializer.size()))
{
    size_type p = 0;
    for (const auto& value : initializer){
        data_ptr_[p ++] = value;
    }
}
        
/* 迭代器构造 */
template<typename T, typename Allocator>
template<typename InputIterator, typename >
inline vector<T, Allocator>::vector(const InputIterator& begin, const InputIterator& end): 
    data_ptr_(allocator_.allocate(end - begin + 1)),
    cur_size_(end - begin + 1),
    max_size_(end - begin + 1)
{
    for (size_t i = 0; i < cur_size_; ++i){
        data_ptr_[i] = *(begin + i);
    }
}

template<typename T, typename Allocator>
inline vector<T, Allocator>::~vector(){
    if (data_ptr_){
        releaseMemory();
        allocator_.deallocate(data_ptr_, max_size_);
    }
    data_ptr_ = nullptr;
}

template<typename T, typename Allocator>
inline void vector<T, Allocator>::reserve(size_type size){
    if (size > max_size_){
        pointer new_pointer = allocator_.allocate(size);
        if constexpr (std::is_trivially_copyable<T>::value) {
            memcpy(new_pointer, data_ptr_, sizeof(T) * cur_size_);
        }
        else{
            for (size_type i = 0; i < cur_size_; ++i) {
                /* Placement New (new (address) T(constructor_args)) */
                new(new_pointer + i) T(mstl::move(data_ptr_[i]));
                data_ptr_[i].~T();
            }
        }
        releaseMemory();
        allocator_.deallocate(data_ptr_, max_size_);
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
inline void vector<T, Allocator>::clear() noexcept{
    for (size_t i = 0; i < cur_size_; ++i){
        data_ptr_[i].~T();
    }
    cur_size_ = 0;
}

template<typename T, typename Allocator>
inline vector<T, Allocator>& vector<T, Allocator>::operator =(const vector<T>& other){
    if (this != &other){
        if (data_ptr_){
            releaseMemory();
        }
        if (max_size_ < other.cur_size_){
            if (data_ptr_){
                allocator_.deallocate(data_ptr_, max_size_);   
            }
            data_ptr_ = allocator_.allocate(other.max_size_);
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
            allocator_.deallocate(data_ptr_, max_size_);
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

template<typename T>
inline bool operator ==(const mstl::vector<T>& lhs, const mstl::vector<T>& rhs){
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
inline void vector<T, Allocator>::push_back(T&& value){
    if (cur_size_ == max_size_){
        this->reserve((max_size_ > 0 ? (max_size_ << 1) : 2));
    }
    new (data_ptr_ + cur_size_) T(mstl::move(value));
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

END_NAMESPACE