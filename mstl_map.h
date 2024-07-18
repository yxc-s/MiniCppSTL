#include "mstl_global.h"
#include "mstl_rbtree.h"
#include "mstl_allocator.h"
#include "mstl_iterator_base.h"


NAMESPACE_MSTL
//TODO:
namespace map_base{
template<typename T, typename U, typename COMPARE_FUNCTION, typename Allocator, const bool IS_MULTI>
class map_impl: public mstl::RedBlackTree<T, U, COMPARE_FUNCTION, Allocator, true, IS_MULTI> {
    using tree_base = mstl::RedBlackTree<T, U, COMPARE_FUNCTION, Allocator, true, IS_MULTI>;
    using node_type = typename tree_base::node_type;
    using node_value_type = typename node_type::value_type;
public:
    using value_type = typename tree_base::node_type;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const pointer;
    using const_reference = const reference;
    using difference_type  =  std::ptrdiff_t;

    /* 当前类和迭代器类里都根据 IS_MULTI 定义了一个相同的迭代器类型，保证在erase的时候进行类型检查不会出错 */
    using iter_type = typename std::conditional_t<IS_MULTI, mstl::multimap_iterator, mstl::map_iterator>;

    map_impl() = default;
    virtual ~map_impl() override = default;

    /* Iterator : 注意！这楼里的value_type是红黑树的node类型，pointer也一样，但是，reference是node里面的data类型!! 并且没有->重载! */
    template<typename ValueType, typename ReferenceType, const bool IS_REVERSE>
    class iterator_impl : public iterator_base<iterator_impl<ValueType, ReferenceType, IS_REVERSE>, ValueType> {
        friend class map_impl<T, U, COMPARE_FUNCTION, Allocator, IS_MULTI>; 
        using map_base_type = map_impl<T, U, COMPARE_FUNCTION, Allocator, IS_MULTI>;
    public:
        using value_type         =   ValueType;
        using pointer            =   value_type*;
        using data_reference     =   ReferenceType;
        using data_pointer       =   typename value_type::pointer;
        using iterator_category  =   mstl::bidirectional_iterator_tag;
        
        /* 该类型用于操作时类型的安全匹配 */
        using iter_type          =   typename std::conditional_t<IS_MULTI, 
            mstl::multimap_iterator, mstl::map_iterator>;

        using this_type = iterator_impl<value_type, data_reference, IS_REVERSE>;
        
        iterator_impl(pointer ptr, map_base_type* map) : ptr_(ptr), parent_map_(map) {}
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
        this_type& operator = (const this_type& other) { 
            ptr_ = other.ptr_; 
            parent_map_ = other.parent_map_; 
            return *this;
        }

    private:
        pointer ptr_;
        map_base_type* parent_map_; /* 注意这里不能用常量指针，否则会影响各种功能的使用~ */

    private:
        /* 找到当前节点的下一个中序遍历节点 */
        void find_next() {
            if (parent_map_->isNil(ptr_)) { 
                if constexpr (IS_REVERSE) {
                    ptr_ = parent_map_->left_most();
                }
                return;
            }
            if (!parent_map_->isNil(ptr_->right_)) {
                ptr_ = ptr_->right_;
                while (!parent_map_->isNil(ptr_->left_)) {
                    ptr_ = ptr_->left_;
                }
            }
            else {
                while (!parent_map_->isNil(ptr_ ->parent_) && ptr_ == ptr_->parent_->right_) {
                    ptr_ = ptr_->parent_;
                }
                ptr_ = ptr_->parent_;
            }
        }

        /* 找到当前节点的上一个中序遍历节点 */
        void find_pre() {
            if (parent_map_->isNil(ptr_)) {
                if constexpr (!IS_REVERSE){
                    ptr_ = parent_map_->right_most();
                }
                return;
            }
            if (!parent_map_->isNil(ptr_->left_)) {
                ptr_ = ptr_->left_;
                while (!parent_map_->isNil(ptr_->right_)) {
                    ptr_ = ptr_->right_;
                }
            }
            else {
                while (!parent_map_->isNil(ptr_->parent_) && ptr_ == ptr_->parent_->left_) {
                    ptr_ = ptr_->parent_;
                }
                ptr_ = ptr_->parent_;
            }
        }
        
    };
   
    /* 四种迭代器类型 , 这里的引用类型使用的是红黑树中node的reference类型! */
    using iterator = iterator_impl<node_type, typename node_type::reference, false>;
    using const_iterator = iterator_impl<const node_type, typename node_type::const_reference, false>;
    using reverse_iterator = iterator_impl<node_type, typename node_type::reference, true>;
    using const_reverse_iterator = iterator_impl<const node_type, typename node_type::const_reference, true>;

    /* 声明为友元，使迭代器到达end时可以合法的移动到一个元素 */
    friend class iterator_impl<node_type, typename node_type::reference, false>;
    friend class iterator_impl<const node_type, typename node_type::const_reference, false>;
    friend class iterator_impl<node_type, typename node_type::reference, true>;
    friend class iterator_impl<const node_type, typename node_type::const_reference, true>;

    /* 正向迭代器相关函数 */
    iterator begin() { return iterator(tree_base::left_most(), this); }
    iterator end()   { return iterator(tree_base::nil_, this); }
    const_iterator cbegin() { return const_iterator(tree_base::left_most(), this); } 
    const_iterator cend() { return const_iterator(tree_base::nil_, this); }

    /* 逆向迭代器相关函数 */
    reverse_iterator rbegin() { return reverse_iterator(tree_base::right_most(), this); }
    reverse_iterator rend() { return reverse_iterator(tree_base::nil_, this); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(tree_base::right_most(), this); }
    const_reverse_iterator crend() { return const_reverse_iterator(tree_base::nil_, this); }

    /* 为什么这个函数不在红黑树里设计了？ 因为该算法返回迭代器，在这里有迭代器的定义，实现起来更方便! */
    /* 查找! */
    iterator lower_bound(const node_value_type& value) {
        return iterator(tree_base::template binary_search_impl<false>(value), this);
    }
    iterator upper_bound(const node_value_type& value) {
        return iterator(tree_base::template binary_search_impl<true>(value), this);
    }

    /* 这里第二个模板参数进行了类型检查，确保迭代器不会混用 */
    template<typename ITER_TYPE, typename = std::enable_if_t<std::is_same_v<typename ITER_TYPE::iter_type, iter_type>>>
    void erase(ITER_TYPE iter) noexcept { 
        if (iter == end()) {
            return;
        }
        tree_base::erase_impl(iter.ptr_); 
    }
    void erase(const node_value_type& value) noexcept { tree_base::erase_impl(value); }
};

}/* end map_base namespace */



/* map : 相同元素最多只有一个的红黑树，这里跟set的区别是多了一个模板参数U（表示value的类型） */
template<typename T, typename U, typename COMPARE_FUNCTION = mstl::less<mstl::pair<T, U>>, typename Allocator = mstl::Allocator<mstl::pair<T, U>>>
class map: public map_base::map_impl<T, U, COMPARE_FUNCTION, Allocator, false> {};



/* multimap : 相同元素可以有很多的红黑树 */
template<typename T, typename U, typename COMPARE_FUNCTION = mstl::less<mstl::pair<T, U>>, typename Allocator = mstl::Allocator<mstl::pair<T, U>>>
class multimap: public map_base::map_impl<T, U, COMPARE_FUNCTION, Allocator, true> {};

END_NAMESPACE