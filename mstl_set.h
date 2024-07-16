#include "mstl_global.h"
#include "mstl_rbtree.h"
#include "mstl_allocator.h"
#include "mstl_iterator_base.h"


NAMESPACE_MSTL

//TODO 迭代器， 目前实现的好像是类似multiset，没有判重的逻辑
template<typename T, typename COMPARE_FUNCTION = mstl::less<T>, typename Allocator = mstl::Allocator<T>>
class set: public mstl::RedBlackTree<T, T, COMPARE_FUNCTION, Allocator, false> {
    using tree_base = mstl::RedBlackTree<T, T, COMPARE_FUNCTION, Allocator, false>;
public:
    using value_type = typename tree_base::node_type;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const pointer;
    using const_reference = const reference;
    using difference_type  =  std::ptrdiff_t;

    set() = default;

    virtual ~set() override = default;

    /* Iterator : 注意！这楼里的value_type是红黑树的node类型，pointer也一样，但是，reference是node里面的data类型!! 并且没有->重载! */
    template<typename ValueType, typename ReferenceType = typename ValueType::reference, const bool IS_REVERSE = false>
    class iterator_impl : public iterator_base<iterator_impl<ValueType, ReferenceType, IS_REVERSE>, ValueType> {
    public:
        using value_type         =   ValueType;
        using pointer            =   value_type*;
        using reference          =   ReferenceType;
        using iterator_category  =   mstl::bidirectional_iterator_tag;


        using this_type = iterator_impl<value_type, reference, IS_REVERSE>;
        
        iterator_impl(pointer ptr) : ptr_(ptr) {}
        ~iterator_impl() override {};

        reference operator*() { return ptr_->data_; }

        //TODO: ++ --逻辑需要重写
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

        iterator_impl& operator--()  { 
            if constexpr (IS_REVERSE){
                ++ptr_;
            }
            else{
                --ptr_; 
            }
            return *this; 
        }
        
        iterator_impl operator--(int)  { 
            iterator_impl new_iter = *this;
            if constexpr (IS_REVERSE) {
                ++(*this);
            } else {
                --(*this);
            }
            return new_iter; 
        }

        //TODO:移除，用distance
        // difference_type operator-(const this_type& other) const override { 
        //     return ptr_ - other.ptr_; 
        // }

        bool operator==(const this_type& other) const { 
            return ptr_ == other.ptr_; 
        }

        bool operator!=(const this_type& other) const { 
            return !(*this == other);
        }

    private:
        pointer ptr_;
    };
   
    /* 四种迭代器类型 , 这里的引用类型使用的是红黑树中node的reference类型! */
    using iterator = iterator_impl<value_type, typename value_type::reference>;
    using const_iterator = iterator_impl<const value_type, typename value_type::const_reference>;
    using reverse_iterator = iterator_impl<value_type, typename value_type::reference, true>;
    using const_reverse_iterator = iterator_impl<const value_type, typename value_type::const_reference, true>;

    /* 正向迭代器相关函数 */
    iterator begin() { return iterator(tree_base::left_most()); }
    iterator end()   { return iterator(nullptr); }
    const_iterator cbegin() { return const_iterator(tree_base::left_most()); } 
    const_iterator cend() { return const_iterator(nullptr); }

    /* 逆向迭代器相关函数 */
    reverse_iterator rbegin() { return reverse_iterator(tree_base::right_most()); }
    reverse_iterator rend() { return reverse_iterator(nullptr); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(tree_base::right_most()); }
    const_reverse_iterator crend() { return const_reverse_iterator(nullptr); }
    
private:

};


END_NAMESPACE