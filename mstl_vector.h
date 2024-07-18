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
    erase考虑要不要加？对于随机访问容器，如果erase中间元素是个大开销，需要考虑一下
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

    using container_type = mstl::container_type_base::_vector_;

    /* 各种构造和析构函数 */
    explicit vector(size_type size = 0);
    vector(size_type size, const value_type& value);
    vector(const vector<value_type>& other);
    vector(vector<value_type>&& other) noexcept;
    vector(const std::initializer_list<value_type>& initializer) noexcept;
    template<typename InputIterator, typename = typename std::enable_if<
        std::is_same_v<typename InputIterator::container_type, container_type>>::type>
    vector(const InputIterator& begin, const InputIterator& end);
    ~vector();

    /* 一些常用函数 */
    void push_back(const value_type& value);
    void push_back(value_type&& value);
    template<typename... Args> void emplace_back(Args&&... args);
    void pop_back();
    void reserve(size_type n);
    vector<value_type, Allocator>& operator =(const vector<value_type>& other);
    vector<value_type, Allocator>& operator =(vector<value_type>&& other) noexcept;
    template<typename U>
    friend bool operator ==(const mstl::vector<U>& lhs, const mstl::vector<U>& rhs);
    template<typename U>
    friend bool operator !=(const mstl::vector<U>& lhs, const mstl::vector<U>& rhs);

    /* 一些类内直接定义的函数 */
    void emplace_back(T&& value)                    { push_back(mstl::forward<T>(value)); }
    reference operator[] (size_type p)              { return data_ptr_[p]; }
    const_reference operator [] (size_type p) const { return data_ptr_[p]; }
    reference front()                               { return data_ptr_[0]; }
    const_reference front()                   const { return data_ptr_[0]; }
    reference back()                                { return data_ptr_[current_size_ - 1]; }
    const_reference back()                    const { return data_ptr_[current_size_ - 1]; }
    size_type size()                            const noexcept { return current_size_; }
    size_type capacity()                        const noexcept { return capacity_; }
    bool empty()                                const noexcept { return static_cast<bool>(current_size_ == 0); }
    void clear()                                      noexcept { call_all_destructors(); }

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
        using container_type = mstl::container_type_base::_vector_;

        
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
        bool operator!=(const this_type& other) const { return !(*this == other);}
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
    iterator end()   { return iterator(data_ptr_ + current_size_); }
    const_iterator cbegin() { return const_iterator(data_ptr_); } 
    const_iterator cend() { return const_iterator(data_ptr_ + current_size_); }

    /* 逆向迭代器相关函数 */
    reverse_iterator rbegin() { return reverse_iterator(data_ptr_ + current_size_ - 1); }
    reverse_iterator rend() { return reverse_iterator(data_ptr_ - 1); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(data_ptr_ + current_size_ - 1); }
    const_reverse_iterator crend() { return const_reverse_iterator(data_ptr_ - 1); }

private:
    Allocator                          allocator_;
    pointer                            data_ptr_;
    size_type                          current_size_;
    size_type                          capacity_;
    
    /* 归还系统当前vector使用的所有内存 */
    void release_memory() {
        call_all_destructors();
        allocator_.deallocate(data_ptr_, capacity_);
        data_ptr_ = 0;
        current_size_ = capacity_ = 0;
    }

    /* 对vector中所有已经构造的对象进行析构 */
    void call_all_destructors(){
        for (size_type i = 0; i < current_size_; ++i){
            data_ptr_[i].~T();
        }
        current_size_ = 0;
    }


};

/* Placement new 表达式: new (address) Type(arguments) */
/* 默认构造函数，按需构造策略 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type size): 
    data_ptr_(allocator_.allocate(size)),
    current_size_(size),
    capacity_(size)
{
    if (capacity_ > 0){
        for (size_type i = 0; i < current_size_; ++i) {
            new(data_ptr_ + i) T();
        }
    }
}

/* 按空间大小与初值构造 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(size_type size, const value_type& value): 
    data_ptr_(allocator_.allocate(size)),
    current_size_(size),
    capacity_(size)
{
    if (capacity_ > 0){
        for (size_type i = 0; i < current_size_; ++i) {
            new(data_ptr_ + i) T(value);
        }
    }
}

/* 拷贝构造 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(const vector<value_type>& other):
    current_size_(other.current_size_),
    capacity_(other.capacity_),
    data_ptr_(allocator_.allocate(capacity_))
{   
    if constexpr (std::is_trivially_copyable<T>::value) {
        memcpy(data_ptr_, other.data_ptr_, sizeof(T) * current_size_);
    }
    else{
        for (size_type i = 0; i < current_size_; ++i) {
            new(data_ptr_ + i) T(other.data_ptr_[i]);
        }
    }    
}

/* 移动构造 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(vector<value_type>&& other) noexcept:
    data_ptr_(other.data_ptr_),
    capacity_(other.capacity_),
    current_size_(other.current_size_)
{   
    other.capacity_ = 0;
    other.current_size_ = 0;
    other.data_ptr_ = nullptr;
}

/* 初始化列表构造 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::vector(const std::initializer_list<value_type>& initializer) noexcept:
    data_ptr_(allocator_.allocate(static_cast<size_type>(initializer.size()))),
    capacity_(static_cast<size_type>(initializer.size())),
    current_size_(static_cast<size_type>(initializer.size()))
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
    current_size_(end - begin + 1),
    capacity_(end - begin + 1)
{
    for (size_t i = 0; i < current_size_; ++i){
        data_ptr_[i] = *(begin + i);
    }
}

/* 析构函数 */
template<typename T, typename Allocator>
inline vector<T, Allocator>::~vector(){
    if (data_ptr_){
        release_memory();
    }
    data_ptr_ = nullptr;
}

/* 向末尾添加左值元素*/
template<typename T, typename Allocator>
inline void vector<T, Allocator>::push_back(const value_type& value){
    if (current_size_ == capacity_){
        this->reserve((capacity_ > 0 ? (capacity_ << 1) : 2));
    }
    new (data_ptr_ + current_size_) T(value);
    ++current_size_;
}

/* 向末尾添加右值元素*/
template<typename T, typename Allocator>
inline void vector<T, Allocator>::push_back(value_type&& value){
    if (current_size_ == capacity_){
        this->reserve((capacity_ > 0 ? (capacity_ << 1) : 2));
    }
    new (data_ptr_ + current_size_) T(mstl::move(value));
    ++current_size_;
}

/* 在末尾根据参数直接构造元素 */
template<typename T, typename Allocator>
template<typename... Args>
inline void vector<T, Allocator>::emplace_back(Args&&... args){
    if (current_size_ == capacity_){
        this->reserve((capacity_ > 0 ? (capacity_ << 1) : 2));
    }
    new(data_ptr_ + current_size_) T(mstl::forward<Args>(args)...);
    ++current_size_;
}


/* 删除最后一个元素 */
template<typename T, typename Allocator>
inline void vector<T, Allocator>::pop_back(){
    if (current_size_ > 0) {
        current_size_ --;
        if constexpr (!std::is_trivially_destructible<T>::value) {
            data_ptr_[current_size_].~T();
        }
    }
}

/* 预分配指定大小内存 */
template<typename T, typename Allocator>
inline void vector<T, Allocator>::reserve(size_type size){
    if (size > capacity_){
        pointer new_pointer = allocator_.allocate(size);
        if constexpr (std::is_trivially_copyable<T>::value) {
            memcpy(new_pointer, data_ptr_, sizeof(T) * current_size_);
        }
        else{
            for (size_type i = 0; i < current_size_; ++i) {
                new(new_pointer + i) T(mstl::move(data_ptr_[i]));
                data_ptr_[i].~T();
            }
        }
        allocator_.deallocate(data_ptr_, capacity_);
        capacity_ = size;
        data_ptr_ = new_pointer;
    }
}

/* 拷贝构造=重载 */
template<typename T, typename Allocator>
inline vector<T, Allocator>& vector<T, Allocator>::operator =(const vector<value_type>& other){
    if (this != &other){
        if (data_ptr_){
            call_all_destructors();
        }
        if (capacity_ < other.current_size_){
            if (data_ptr_){
                release_memory();
            }
            data_ptr_ = allocator_.allocate(other.capacity_);
            capacity_ = other.capacity_;
        }
        current_size_ = other.current_size_;
        if constexpr (std::is_trivially_copyable<value_type>::value) {
            memcpy(data_ptr_, other.data_ptr_, sizeof(value_type) * other.current_size_);
        }
        else{
            for (size_type i = 0; i < current_size_; ++i) {
                /* Placement New (new (address) T(constructor_args)) */
                new(data_ptr_ + i) T(other.data_ptr_[i]);
            }
        }
    }
    return *this;
}

/* 移动构造=重载 */
template<typename T, typename Allocator>
inline vector<T, Allocator>& vector<T, Allocator>::operator =(vector<value_type>&& other) noexcept{
    if (this != &other){
        if (data_ptr_){
            release_memory();
        }
        data_ptr_ = other.data_ptr_;
        current_size_ = other.current_size_;
        capacity_ = other.capacity_;
        other.data_ptr_ = nullptr;
        other.capacity_ = 0;
        other.current_size_ = 0;
    }
    return *this;
}

/* 检查两个vector是否相等 */
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

/* 检查两个vector是否不相等 */
template<typename T>
inline bool operator !=(const mstl::vector<T>& lhs, const mstl::vector<T>& rhs){
    return !(lhs == rhs);
}


END_NAMESPACE