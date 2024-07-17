#include "mstl_global.h"
#include "mstl_rbtree.h"
#include "mstl_allocator.h"
#include "mstl_iterator_base.h"


NAMESPACE_MSTL

namespace map_base{
template<typename T, typename U, typename COMPARE_FUNCTION, typename Allocator, const bool IS_MULTI>
class map_impl: public mstl::RedBlackTree<T, U, COMPARE_FUNCTION, Allocator, true, IS_MULTI> {
    using tree_base = mstl::RedBlackTree<T, U, COMPARE_FUNCTION, Allocator, true, IS_MULTI>;
public:
    using value_type = typename tree_base::node_type;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const pointer;
    using const_reference = const reference;
    using difference_type  =  std::ptrdiff_t;

    map_impl() = default;
    virtual ~map_impl() override = default;

    /* Iterator : 注意！这楼里的value_type是红黑树的node类型，pointer也一样，但是，reference是node里面的data类型!! 并且没有->重载! */
    template<typename ValueType, typename ReferenceType = typename ValueType::reference, const bool IS_REVERSE = false>
    class iterator_impl : public iterator_base<iterator_impl<ValueType, ReferenceType, IS_REVERSE>, ValueType> {
    public:
        using value_type         =   ValueType;
        using pointer            =   value_type*;
        using data_reference     =   ReferenceType;
        using data_pointer       =   typename value_type::pointer;
        using iterator_category  =   mstl::bidirectional_iterator_tag;


        using this_type = iterator_impl<value_type, data_reference, IS_REVERSE>;
        
        iterator_impl(pointer ptr) : ptr_(ptr) {}
        ~iterator_impl() override {};

        data_reference operator*() { return ptr_->data_; }
        data_pointer operator->()  { return &(ptr_->data_); }

        iterator_impl& operator++() override { 
            if constexpr (IS_REVERSE){
                find_pre();
            }
            else{
                find_next();
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
                find_next();
            }
            else{
                find_pre();
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

        /* 迭代器比较运算符 */
        bool operator==(const this_type& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const this_type& other) const { return !(*this == other); }

    private:
        pointer ptr_;

    private:
        /* 找到当前节点的下一个中序遍历节点 */
        void find_next() {
            if (!ptr_) { 
                return;
            }
            if (ptr_->right_) {
                ptr_ = ptr_->right_;
                while (ptr_->left_) {
                    ptr_ = ptr_->left_;
                }
            }
            else {
                while (ptr_ ->parent_ && ptr_ == ptr_->parent_->right_) {
                    ptr_ = ptr_->parent_;
                }
                ptr_ = ptr_->parent_;
            }
        }

        /* 找到当前节点的上一个中序遍历节点 */
        void find_pre() {
            if (!ptr_) {
                return;
            }
            if (ptr_->left_) {
                ptr_ = ptr_->left_;
                while (ptr_->right_) {
                    ptr_ = ptr_->right_;
                }
            }
            else {
                while (ptr_->parent_ && ptr_ == ptr_->parent_->left_) {
                    ptr_ = ptr_->parent_;
                }
                ptr_ = ptr_->parent_;
            }
        }
        
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
};

}/* end map_base namespace */



/* map : 相同元素最多只有一个的红黑树，这里跟set的区别是多了一个模板参数U（表示value的类型） */
template<typename T, typename U, typename COMPARE_FUNCTION = mstl::less<mstl::pair<T, U>>, typename Allocator = mstl::Allocator<mstl::pair<T, U>>>
class map: public map_base::map_impl<T, U, COMPARE_FUNCTION, Allocator, false> {};



/* multimap : 相同元素可以有很多的红黑树 */
template<typename T, typename U, typename COMPARE_FUNCTION = mstl::less<mstl::pair<T, U>>, typename Allocator = mstl::Allocator<mstl::pair<T, U>>>
class multimap: public map_base::map_impl<T, U, COMPARE_FUNCTION, Allocator, true> {};

END_NAMESPACE