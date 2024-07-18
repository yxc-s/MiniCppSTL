#pragma once

#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_iterator_base.h" 

#include <type_traits>

/* 
TODO:
    reserve是否应该分为front和back？
    erase考虑要不要加？对于随机访问容器，如果erase中间元素是个大开销，需要考虑一下
*/
NAMESPACE_MSTL

template<typename T, typename Allocator = mstl::Allocator<T>, mstl::size_t BLOCK_SIZE = 512, mstl::size_t INITIAL_INDEX = BLOCK_SIZE / 2>
class deque{
    static constexpr const size_t INITIAL_CAPACITY =   BLOCK_SIZE - INITIAL_INDEX; /* 256 : 下标256 ~ 511*/
    /* 使用这个东西的好处就是，确保另一个deque的块大小，起始坐标大小都是相同的 */
    using this_deque_type = mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>;
public:
    /* STL format */
    using value_type       =  T;
    using pointer          =  T*;
    using reference        =  T&;
    using const_pointer    =  const T*;
    using const_reference  =  const T&;
    using size_type        =  mstl::size_t;
    
    /* 指向内存块的allocator类型 */
    using block_allocator_type = typename Allocator::template rebind<pointer>::other;

    using container_type = mstl::container_type_base::_deque_;

    /* 各种构造和析构函数 */
    explicit deque(size_type size = 0);
    deque(size_type size, const value_type& value);
    deque(const this_deque_type& other);
    deque(this_deque_type&& other);
    deque(const std::initializer_list<value_type>& initializer) noexcept;
    template<typename InputIterator>
    deque(const InputIterator& begin, const InputIterator& end);
    ~deque();

    /* 一些常用函数 */
    void push_front(const T& value);
    void push_front(T&& value);
    template<typename... Args> void emplace_front(Args&&... args);
    void push_back(const T& value);
    void push_back(T&& value);
    void pop_back();
    void pop_front();
    template<typename... Args> void emplace_back(Args&&... args);
    void reserve(size_type size);

    /* 一些运算符重载 */
    mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>& operator =(const this_deque_type& other);
    mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>& operator =(this_deque_type&& other) noexcept;
    template<typename U>
    friend bool operator ==(const mstl::deque<U>& lhs, const mstl::deque<U>& rhs);
    template<typename U>
    friend bool operator !=(const mstl::deque<U>& lhs, const mstl::deque<U>& rhs);

    /* 一些类内直接定义的函数 */
    void emplace_front(T&& value)                     { push_front(mstl::forward<T>(value)); }
    void emplace_back(T&& value)                      { push_back(mstl::forward<T>(value)); }
    reference operator[] (size_type p)                { return *(get_position_pointer(front_index_ + p)); } 
    const_reference operator [] (size_type p)   const { return *(get_position_pointer(front_index_ + p)); }
    reference front()                                 { return *(get_position_pointer(front_index_)); }
    const_reference front()                     const { return *(get_position_pointer(front_index_)); }
    reference back()                                  { return *(get_position_pointer(back_index_ - 1)); }
    const_reference back()                      const { return *(get_position_pointer(back_index_ - 1)); }
    size_type size()                            const noexcept { return back_index_ - front_index_; }
    size_type capacity()                        const noexcept { return BLOCK_SIZE * num_blocks_; }
    bool empty()                                const noexcept { return static_cast<bool> (back_index_ - front_index_ == 0);}
    void clear()                                      noexcept { call_all_destructors(); }

    /* Iterator */
    /* 这里的设计引用了一个相关deque父类，在返回内存数据时，要先通过父类计算出下标对应的指针 */
    template<typename ValueType = T, typename PointerType = ValueType*, typename ReferenceType = ValueType&, const bool IS_REVERSE = false>
    class iterator_impl : public iterator_base<iterator_impl<ValueType, PointerType, ReferenceType, IS_REVERSE>, ValueType> {
        using this_iter_type = iterator_impl<value_type, pointer, reference, IS_REVERSE>;    
    public:
        using value_type         =   ValueType;
        using pointer            =   PointerType;
        using reference          =   ReferenceType;
        using difference_type    =   std::ptrdiff_t;
        using iterator_category  =   mstl::random_access_iterator_tag;

        using container_type = mstl::container_type_base::_deque_;
        
        iterator_impl(size_t index, mstl::deque<T>* deque) : index_(index), deque_(deque) {}

        reference operator*() const { return *(deque_->get_position_pointer(index_)); }
        pointer operator->()  { return deque_->get_position_pointer(index_); }

        iterator_impl& operator++() override { 
            if constexpr (IS_REVERSE){
                -- index_;
            }
            else{
                ++ index_; 
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
                ++ index_;
            }
            else{
                -- index_; 
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
                return iterator_impl(index_ - offset, deque_);
            } else {
                return iterator_impl(index_ + offset, deque_);
            }
        }

        iterator_impl operator-(difference_type offset) const { 
            if constexpr (IS_REVERSE) {
                return iterator_impl(index_ + offset, deque_);
            } else {
                return iterator_impl(index_ - offset, deque_);
            }
        }

        difference_type operator-(const this_iter_type& other) const { return index_ - other.index_; }
        bool operator==(const this_iter_type& other) const { return index_ == other.index_; }
        bool operator!=(const this_iter_type& other) const { return !(*this == other); }
        bool operator <(const this_iter_type& other) const{ return index_ < other.index_; }
        bool operator <=(const this_iter_type& other) const{ return index_ <= other.index_; }
        bool operator >(const this_iter_type& other) const{ return !(*this <= other); }
        bool operator >=(const this_iter_type& other) const{ return !(*this < other); }
        this_iter_type& operator =(const this_iter_type& other) {
            if (this != &other) {
                index_ = other.index_;
                deque_ = other.deque_;
            }
            return *this;
        }

    private:
        size_t index_;
        mstl::deque<T>* deque_;
    };
    
    /* 四种迭代器类型 */
    using iterator = iterator_impl<value_type>;
    using const_iterator = iterator_impl<const value_type, const_pointer, const_reference>;
    using reverse_iterator = iterator_impl<value_type, pointer, reference, true>;
    using const_reverse_iterator = iterator_impl<const value_type, const_pointer, const_reference, true>;

    /* 将各种迭代器的类型声明为友元类，方便后续迭代器通过对应的下标直接计算指针地址 */
    friend class iterator_impl<value_type>;
    friend class iterator_impl<const value_type, const_pointer, const_reference>;
    friend class iterator_impl<value_type, pointer, reference, true>;
    friend class iterator_impl<const value_type, const_pointer, const_reference, true>;

    /* 正向迭代器相关函数 */
    iterator begin() { return iterator(front_index_, this); }
    iterator end()   { return iterator(back_index_, this); }
    const_iterator cbegin() { return const_iterator(front_index_, this); } 
    const_iterator cend() { return const_iterator(back_index_, this); }

    /* 逆向迭代器相关函数 */
    reverse_iterator rbegin() { return reverse_iterator(back_index_ - 1, this); }
    reverse_iterator rend() { return reverse_iterator(front_index_ - 1, this); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(back_index_ - 1, this); }
    const_reverse_iterator crend() { return const_reverse_iterator(front_index_ - 1, this); }
    

private:
    size_type              num_blocks_;
    size_type              front_index_;
    size_type              back_index_;
    Allocator              allocator_;
    block_allocator_type   block_allocator_;
    pointer*               block_ptr_;
    
    /* 根据index计算对应的指针 */
    pointer get_position_pointer (size_t index) { 
        size_t block_id = index / BLOCK_SIZE;
        return block_ptr_[block_id] + (index % BLOCK_SIZE);
    }

    const_pointer get_position_pointer (size_t index) const{ 
        size_t block_id = index / BLOCK_SIZE;
        return block_ptr_[block_id] + (index % BLOCK_SIZE);
    }


    /* 在deque前面增加内存块 */
    void add_front_block(){
        pointer* new_block_ptr_ = block_allocator_.allocate(num_blocks_ + 1);
        for (size_t i = 1; i <= num_blocks_; ++i){
            new_block_ptr_[i] = block_ptr_[i - 1];
        }
        new_block_ptr_[0] = allocator_.allocate(BLOCK_SIZE);
        num_blocks_ ++;
        front_index_ += BLOCK_SIZE;
        back_index_ += BLOCK_SIZE;
        block_ptr_ = new_block_ptr_;
    }

    /* 在deque后面增加内存块 */
    void add_back_block(){
        pointer* new_block_ptr_ = block_allocator_.allocate(num_blocks_ + 1);
        for (size_t i = 0; i < num_blocks_; ++i){
            new_block_ptr_[i] = block_ptr_[i];
        }
        new_block_ptr_[num_blocks_] = allocator_.allocate(BLOCK_SIZE);
        num_blocks_ ++;
        block_ptr_ = new_block_ptr_;
    }

    /* 归还系统当前deque使用的所有内存 */
    void release_memory(){
        call_all_destructors();
        for (size_t i = 0; i < num_blocks_; ++i){
            allocator_.deallocate(block_ptr_[i], BLOCK_SIZE);
        }
        block_allocator_.deallocate(block_ptr_, num_blocks_);
        front_index_ = back_index_ = INITIAL_INDEX;
        num_blocks_ = 0;
        block_ptr_ = nullptr;
    }

    /* 对deque中所有已经构造的对象进行析构 */
    void call_all_destructors(){
        while (front_index_ < back_index_){
            (*get_position_pointer(front_index_)).~T();
            ++front_index_;
        }
        front_index_ = back_index_ = INITIAL_INDEX;
    }
};

/* 默认构造函数，按需构造策略 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(size_type size) : 
    front_index_(INITIAL_INDEX),
    back_index_(front_index_ + size),
    num_blocks_((INITIAL_CAPACITY + size + BLOCK_SIZE - 1) / BLOCK_SIZE),
    block_ptr_(block_allocator_.allocate(num_blocks_))
{   
    for (size_t i = 0; i < num_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T();
    }
}

/* 按空间大小与初值构造 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(size_type size, const value_type& value) : 
    front_index_(INITIAL_INDEX),
    back_index_(front_index_ + size),
    num_blocks_((INITIAL_CAPACITY + size + BLOCK_SIZE - 1) / BLOCK_SIZE),
    block_ptr_(block_allocator_.allocate(num_blocks_))
{
    for (size_t i = 0; i < num_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(value);
    }
}

/* 拷贝构造 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(const this_deque_type& other):
    front_index_(other.front_index_),
    back_index_(other.back_index_),
    num_blocks_(other.num_blocks_),
    block_ptr_(block_allocator_.allocate(num_blocks_))
{
    for (size_t i = 0; i < num_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(*other.get_position_pointer(i));
    }
}

/* 移动构造 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(this_deque_type&& other):
    front_index_(other.front_index_),
    back_index_(other.back_index_),
    num_blocks_(other.num_blocks_),
    block_ptr_(other.block_ptr_)
{
    other.front_index_ = other.back_index_ = INITIAL_INDEX;
    other.num_blocks_ = 0;
    other.block_ptr_ = nullptr;  
}

/* 初始化列表构造 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(const std::initializer_list<value_type>& initializer) noexcept:
    front_index_(INITIAL_INDEX),
    back_index_(front_index_ + initializer.size()),
    num_blocks_((INITIAL_CAPACITY + initializer.size() + BLOCK_SIZE - 1) / BLOCK_SIZE),
    block_ptr_(block_allocator_.allocate(num_blocks_))
{   
    auto it = initializer.begin();
    for (size_t i = 0; i < num_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(*it);
        it ++;
    }
}

/* 迭代器构造 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
template<typename InputIterator>
mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::deque(const InputIterator& begin, const InputIterator& end):
    front_index_(INITIAL_INDEX),
    back_index_(front_index_ + (end - begin)),
    num_blocks_((INITIAL_CAPACITY + (end - begin) + BLOCK_SIZE - 1) / BLOCK_SIZE),
    block_ptr_(block_allocator_.allocate(num_blocks_ * sizeof(pointer)))
{   
    for (size_t i = 0; i < num_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(*(begin + i - front_index_));
    }
}

/* 析构函数 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::~deque(){
    if (block_ptr_ != nullptr){
        release_memory();
    }
}

/* 向首部添加左值元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::push_front(const value_type& value){
    if (!front_index_){
        add_front_block();
    }
    front_index_ --;
    new (get_position_pointer(front_index_)) T(value);
}

/* 向首部添加右值元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::push_front(value_type&& value){
    if (!front_index_){
        add_front_block();
    }
    front_index_ --;
    new (get_position_pointer(front_index_)) T(mstl::move(value));
}

/* 在首部根据参数直接构造元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
template<typename... Args>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::emplace_front(Args&&... args){
    if (!front_index_){
        add_front_block();
    }
    front_index_ --;
    new(get_position_pointer(front_index_)) T(mstl::forward<Args>(args)...);
}

/* 向末尾添加左值元素，注意back_index_是最后一个元素下标的下一位  */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::push_back(const value_type& value){
    if (back_index_ % BLOCK_SIZE == 0 && back_index_ / BLOCK_SIZE == num_blocks_){
        add_back_block();
    }
    new(get_position_pointer(back_index_)) T(value);
    back_index_ ++;
}

/* 向末尾添加右值元素*/
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::push_back(value_type&& value){
    if (back_index_ % BLOCK_SIZE == 0 && back_index_ / BLOCK_SIZE == num_blocks_){
        add_back_block();
    }
    new(get_position_pointer(back_index_)) T(mstl::move(value));
    back_index_ ++;
}

/* 在末尾根据参数直接构造元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
template<typename... Args>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::emplace_back(Args&&... args){
    if ((back_index_ % BLOCK_SIZE == 0 && back_index_ / BLOCK_SIZE == num_blocks_)){
        add_back_block();
    }
    new(get_position_pointer(back_index_)) T(mstl::forward<Args>(args)...);
    back_index_ ++;
}

/* 删除最后一个元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::pop_back(){
    (get_position_pointer(back_index_ - 1))->~T();
    back_index_ --;
}

/* 删除第一个元素 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::pop_front(){
    (get_position_pointer(front_index_))->~T();
    front_index_ ++;
}

/* 预分配指定大小内存 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline void mstl::deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::reserve(size_type size){
    if (BLOCK_SIZE * num_blocks_ < size){
        size_t new_block_size = (INITIAL_CAPACITY + size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        pointer* new_block_pointer = block_allocator_.allocate(new_block_size);
        for (size_t i = 0; i < new_block_size; ++i){
            if (i < num_blocks_) {
                new_block_pointer[i] = block_ptr_[i];
            }
            else{
                new_block_pointer[i] = allocator_.allocate(BLOCK_SIZE);
            }
        }
        block_ptr_ = new_block_pointer;
        num_blocks_ = new_block_size;
    }
}

/* 拷贝构造=重载 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>& deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::operator =(const this_deque_type& other){
    if (this != &other){
        if (block_ptr_){
            release_memory();
        }
        reserve(other.back_index_ - other.front_index_);
        for (size_type i = other.front_index_; i < other.back_index_; ++i) {
            new(get_position_pointer(i)) T(*(other.get_position_pointer(i)));
        }
        front_index_ = other.front_index_;
        back_index_ = other.back_index_;
        //cur_blocks_和block_pointer_在reserve的时候已更新.
    }
    return *this;
}

/* 移动构造=重载 */
template<typename T, typename Allocator, mstl::size_t BLOCK_SIZE, mstl::size_t INITIAL_INDEX>
inline deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>& deque<T, Allocator, BLOCK_SIZE, INITIAL_INDEX>::operator =(this_deque_type&& other) noexcept{
    if (this != &other){
        if (block_ptr_){
            release_memory();
        }
        block_ptr_ = other.block_ptr_;
        front_index_ = other.front_index_;
        back_index_ = other.back_index_;
        num_blocks_ = other.num_blocks_;
        other.block_ptr_ = nullptr;
        other.front_index_ = other.back_index_ = INITIAL_INDEX - 1;
        other.num_blocks_ = 0;
    }
    return *this;
}

/* 检查两个deque是否相等 */
template<typename T>
inline bool operator ==(const mstl::deque<T>& lhs, const mstl::deque<T>& rhs){
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

/* 检查两个deque是否不相等 */
template<typename T>
inline bool operator !=(const mstl::deque<T>& lhs, const mstl::deque<T>& rhs){
    return !(lhs == rhs);
}


END_NAMESPACE