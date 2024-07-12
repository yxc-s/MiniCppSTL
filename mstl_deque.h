#pragma once

#include "mstl_global.h"
#include "mstl_allocator.h"
#include "mstl_memory.h" //unique_ptr
#include "mstl_iterator_base.h" 

#include <type_traits>

/* 
TODO: iterator 和一些小的函数
    emplace
    rebind的alloccator是否可靠，待验证
*/
NAMESPACE_MSTL

template<typename T, typename Allocator = mstl::Allocator<T>>
class deque{
    static constexpr const size_t BLOCK_SIZE = 512;
public:
    /* STL format */
    using value_type       =  T;
    using pointer          =  T*;
    using reference        =  T&;
    using const_pointer    =  const T*;
    using const_reference  =  const T&;
    using size_type        =  mstl::size_t;

    using block_allocator_type = typename Allocator::template rebind<pointer>::other;

    explicit deque(size_type size = 0);
    deque(size_type size, const value_type& value);
    deque(const deque<value_type>& other);
    deque(deque<value_type>&& other);
    deque(const std::initializer_list<value_type>& initializer) noexcept;
    /* 迭代器构造 */
    template<typename InputIterator, typename = typename std::enable_if<
        std::is_same_v<typename InputIterator::value_type, value_type>>::type>
    deque(const InputIterator& begin, const InputIterator& end);
    ~deque();

    void push_front(const T& value);
    void push_front(T&& value);

    void push_back(const T& value);
    void push_back(T&& value);

    void pop_back();
    void pop_front();

    void reserve(size_type size);

    void clear();

    deque<value_type, Allocator>& operator =(const deque<value_type>& other);
    deque<value_type, Allocator>& operator =(deque<value_type>&& other) noexcept;

    template<typename U>
    friend bool operator ==(const mstl::deque<U>& lhs, const mstl::deque<U>& rhs);

    reference operator[] (size_type p);
    const_reference operator [] (size_type p) const;

    reference front();
    const_reference front()                   const;

    reference back()                                ;
    const_reference back()                    const ;

    size_type size()                            const noexcept;
    size_type capacity()                        const noexcept;
    bool empty()                                const noexcept;
    
    /* iterator*/

    

private:
    size_type cur_blocks_;
    size_type front_index_;
    size_type back_index_;
    size_type cur_size_;
    Allocator allocator_;
    block_allocator_type block_allocator_;
    mstl::unique_ptr<pointer[]> block_ptr_;
    
    constexpr pointer get_position_pointer (size_t position) const { 
        size_t block_id = position / BLOCK_SIZE;
        return block_ptr_[block_id] + (position % BLOCK_SIZE);
    }

    pointer* add_front_block(){
        pointer* res = block_allocator_.allocate(cur_blocks_ + 1);
        for (size_t i = 1; i <= cur_blocks_; ++i){
            res[i] = block_ptr_[i - 1];
        }
        res[0] = allocator_.allocate(BLOCK_SIZE);
        cur_blocks_ ++;
        front_index_ += BLOCK_SIZE;
        back_index_ += BLOCK_SIZE;
        return res;
    }

    pointer* add_back_block(){
        pointer* res = block_allocator_.allocate(cur_blocks_ + 1);
        for (size_t i = 0; i < cur_blocks_; ++i){
            res[i] = block_ptr_[i];
        }
        res[cur_blocks_] = allocator_.allocate(BLOCK_SIZE);
        cur_blocks_ ++;
        return res;
    }
};

/* 按需构造策略 */
template<typename T, typename Allocator>
deque<T, Allocator>::deque(size_type size) : 
    front_index_(size > 0 ? BLOCK_SIZE - 1 : 0),
    back_index_(front_index_ + size),
    cur_blocks_(1 + (size + BLOCK_SIZE - 1) / BLOCK_SIZE),
    cur_size_(size),
    block_ptr_(block_allocator_.allocate(cur_blocks_))
{   
    for (size_t i = 0; i < cur_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T();
    }
}


template<typename T, typename Allocator>
deque<T, Allocator>::deque(size_type size, const T& value) : 
    front_index_(BLOCK_SIZE - 1),
    back_index_(front_index_ + size),
    cur_blocks_(1 + (size + BLOCK_SIZE - 1) / BLOCK_SIZE),
    cur_size_(size),
    block_ptr_(block_allocator_.allocate(cur_blocks_))
{
    for (size_t i = 0; i < cur_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(value);
    }
}

template<typename T, typename Allocator>
deque<T, Allocator>::deque(const deque<T>& other):
    front_index_(other.front_index_),
    back_index_(other.back_index_),
    cur_blocks_(other.cur_blocks_),
    cur_size_(other.cur_size_),
    block_ptr_(block_allocator_.allocate(cur_blocks_))
{
    for (size_t i = 0; i < cur_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_; i < back_index_; ++i){
        new(get_position_pointer(i)) T(*other.get_position_pointer(i));
    }
}

template<typename T, typename Allocator>
deque<T, Allocator>::deque(deque<T>&& other):
    front_index_(other.front_index_),
    back_index_(other.back_index_),
    cur_blocks_(other.cur_blocks_),
    cur_size_(other.cur_size_),
    block_ptr_(other.block_ptr_)
{
    other.front_index_ = 0;
    other.back_index_ =  0;
    other.cur_blocks_ = 0;
    other.cur_size_ = 0;
    other.block_ptr_ = nullptr;  
}

template<typename T, typename Allocator>
deque<T, Allocator>::deque(const std::initializer_list<T>& initializer) noexcept:
    front_index_(BLOCK_SIZE - 1),
    back_index_(front_index_ + initializer.size()),
    cur_blocks_(1 + (initializer.size() + BLOCK_SIZE - 1) / BLOCK_SIZE),
    cur_size_(initializer.size()),
    block_ptr_(block_allocator_.allocate(cur_blocks_))
{   
    for (size_t i = 0; i < cur_blocks_; ++i){
        block_ptr_[i] = allocator_.allocate(BLOCK_SIZE);
    }
    for (size_t i = front_index_, j = 0; i < back_index_; ++i, ++j){
        new(get_position_pointer(i)) T(initializer[j]);
    }
}


/* 是否需先实现distance函数？ 这里应该是不能直接迭代器相减获得数量了 */

// template<typename T, typename Allocator>
// template<typename InputIterator, typename >
// deque<T, Allocator>::deque(const InputIterator& begin, const InputIterator& end):
//     front_index_(BLOCK_SIZE - 1),
//     back_index_(front_index_ + size),
//     cur_blocks_(1 + (...) / BLOCK_SIZE),
//     cur_size_(size),
//     block_ptr_(allocator_.allocate(cur_blocks_ * sizeof(pointer)))
// {

// }


template<typename T, typename Allocator>
void deque<T, Allocator>::push_front(const T& value){
    if (!front_index_){
        add_front_block();
    }
    front_index_ --;
    new (get_position_pointer(front_index_)) T(value);
}

template<typename T, typename Allocator>
void deque<T, Allocator>::push_front(T&& value){
    if (!front_index_){
        add_front_block();
    }
    front_index_ --;
    new (get_position_pointer(front_index_)) T(mstl::move(value));
}

template<typename T, typename Allocator>
void deque<T, Allocator>::push_back(const T& value){
    if (back_index_ % BLOCK_SIZE == 0 && back_index_ / BLOCK_SIZE == cur_blocks_){
        add_back_block();
    }
    new(get_position_pointer(back_index_)) T(value);
    back_index_ ++;
}

template<typename T, typename Allocator>
void deque<T, Allocator>::push_back(T&& value){
    if (back_index_ % BLOCK_SIZE == 0 && back_index_ / BLOCK_SIZE == cur_blocks_){
        add_back_block();
    }
    new(get_position_pointer(back_index_)) T(mstl::move(value));
    back_index_ ++;
}


END_NAMESPACE