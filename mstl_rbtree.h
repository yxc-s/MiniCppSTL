#pragma once

#include <type_traits>   //std::conditional_t
#include "mstl_pair.h"

/*
红黑树参考的：https://blog.csdn.net/qq_40843865/article/details/102498310
https://www.cnblogs.com/qingergege/p/7351659.html
*/

NAMESPACE_MSTL 

//TODO:这里是不是不需要友元类？ 
// template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP>
// class RedBlackTree;

template<typename T, typename U, const bool IS_MAP>//, typename Allocator, typename Compare>
class RbtNode{
  //  friend class RedBlackTree<T, U, Compare, Allocator, IS_MAP>;
public:
    using value_type      =  typename std::conditional_t<IS_MAP, mstl::pair<T, U>, T>;
    using const_value_type = const value_type;
    using pointer         =  value_type*;
    using const_pointer   =  const value_type*;
    using reference       =  value_type&;
    using const_reference =  const value_type&;

    explicit RbtNode(const value_type& value) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(value),
        is_black_(false)
    {}

    explicit RbtNode(value_type&& value) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(mstl::move(value)),
        is_black_(false)
    {}

    RbtNode(const RbtNode& other) :
        left_(other.left_), 
        right_(other.right_), 
        parent_(other.parent_), 
        data_(other.data_),
        is_black_(other.is_black_)
    {}

    RbtNode(RbtNode&& other) :
        left_(other.left_), 
        right_(other.right_), 
        parent_(other.parent_), 
        data_(mstl::move(other.data_)),
        is_black_(other.is_black_)
    {}

    ~RbtNode() = default;

    RbtNode& operator =(const RbtNode& other) {
        left_ = other.left_;
        right_ = other.right_;
        parent_ = other.parent_;
        data_ = other.data_;
        is_black_ = other.is_black_;
    }

    RbtNode& operator =(RbtNode&& other) {
        left_ = other.left_;
        right_ = other.right_;
        parent_ = other.parent_;
        data_ = mstl::move(other.data_);
        is_black_ = other.is_black_;
    }

    reference operator *() { return data_; } 
    const_reference operator*() const { return data_; }

//private:
    RbtNode*   left_;
    RbtNode*   right_;
    RbtNode*   parent_;
    bool       is_black_;
    value_type  data_;

};



template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
class RedBlackTree{
public:
    using key_type                 =    T;
    using value_type               =    U;
    using size_type                =    mstl::size_t;
    using node_type                =    RbtNode<T, U, IS_MAP>;
    //using node_type                =    RbtNode<T, U, IS_MAP, Allocator, Compare>;
    using compare_function_type    =    Compare;
    using allocator_type           =    typename Allocator::template rebind<node_type>::other;



    RedBlackTree() : nil_(nullptr), root_(nullptr), current_size_(0){}

    virtual ~RedBlackTree(){
        destory(root_);
    }

#if 0
    // template<bool B = IS_MAP, typename = std::enable_if_t<!B>>
    // void insert(const key_type& key);

    // template<bool B = IS_MAP, typename = std::enable_if_t<!B>>
    // void insert(key_type&& key);

    // template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    // void insert(const mstl::pair<key_type, value_type>& value);

    // template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    // void insert(mstl::pair<key_type, value_type>&& value);

#endif
    
    /* 这里注意要去除一下V的引用才可以，不然只能接受右值(签名增加了enable_if可能会导致只接受右值) */
    template<typename V, typename = std::enable_if_t<std::is_same_v<std::remove_reference_t<V>, typename node_type::value_type>>>
    void insert(V&& value);

    template<typename V, typename = std::enable_if_t<std::is_same_v<V, typename node_type::value_type>>>
    void erase(const V& value);

    size_type size() const noexcept { return current_size_; }


private:
    node_type*              nil_;
    node_type*              root_;
    size_type               current_size_;
    compare_function_type   compare_function_;
    allocator_type          allocator_;

    /* 后序遍历，析构时调用 */
    void destory(node_type* node) {
        if (node != nullptr) {
            destory(node->left_);
            destory(node->right_);
            node->~node_type();
            allocator_.deallocate(node, 1);
        }
    }

protected:
    
    /* 一些小函数 */
    bool is_left(node_type* x) const noexcept { return (x->parent_)->left_ == x; }
    bool is_right(node_type* x) const noexcept { return (x->parent_)->right_ == x; }
    bool has_brother(node_type* x) const noexcept { return is_left(x) ? x->parent_->right_ != nil_ : x->parent_->left_ != nil_; }
    node_type* get_brother(node_type* x) const noexcept { return is_left(x) ? x->parent_->right_ : x->parent_->left_;  }
    bool isNil(node_type* x) const noexcept { return x == nil_;}

    /* rotate 函数声明 */
    void rotate_left(node_type* x);
    void rotate_right(node_type* x);

    /* 插入新元素逻辑的具体实现 */
    void insert_new_node_impl(node_type* x);
    void insert_fix_impl(node_type* x) noexcept;
    
    /* 删除元素逻辑的具体实现 */
    void erase_node_impl(node_type* node);

    /* 查找指定数值节点 */
    template<typename V>
    node_type* find_node(const V& value){
        node_type* cur = root_;
        while (cur != nil_){
            if (cur->data_ == value) {
                return cur;
            }
            if (compare_function_(cur->data_, value)){
                cur = cur->right_;
            }
            else{
                cur = cur->left_;
            }
        }
        return nullptr;
    }

    /* 四个获取边界元素函数，用于获取迭代器时调用 */
    node_type* left_most() noexcept {
        node_type* cur = root_;
        while (cur && !isNil(cur->left_)) {
            cur = cur->left_;
        }
        return cur;
    }
    const node_type* left_most() const noexcept {
        node_type* cur = root_;
        while (cur && !isNil(cur->right_)) {
            cur = cur->right_;
        }
        return cur;
    }
    node_type* right_most() noexcept {
        node_type* cur = root_;
        while (cur && !isNil(cur->right_)) {
            cur = cur->right_;
        }
        return cur;
    }
    const node_type* right_most() const noexcept {
        node_type* cur = root_;
        while (cur && !isNil(cur->right_)) {
            cur = cur->right_;
        }
        return cur;
    }


};

/* 左旋：右孩子y变为当前节点x的父亲节点，x成为y的左孩子，y之前的左孩子成为x的右孩子 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::rotate_left(node_type* x){
    node_type* y = x->right_;
    x->right_ = y->left_;
    if (!isNil(y->left_)){
        y->left_->parent_ = x;
    }
    if (isNil(x->parent_)){
        root_ = y;
    }
    else if (is_left(x)){
        x->parent_->left_ = y;
    }
    else{
        x->parent_->right_ = y;
    }
    y->parent_ = x->parent_;
    y->left_ = x;
    x->parent_ = y;
}

/* 右旋: 左孩子y变为当前节点x的父亲节点，x成为y的右孩子，y之前的右孩子成为x的左孩子 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::rotate_right(node_type* x){
    node_type* y = x->left_;
    x->left_ = y->right_;
    if (!isNil(y->right_)){
        y->right_->parent_ = x;
    }
    if (isNil(x->parent_)){
        root_ = y;
    }
    else if (is_left(x)){
        x->parent_->left_ = y;
    }
    else{
        x->parent_->right_ = y;
    }
    y->parent_ = x->parent_;
    y->right_ = x;
    x->parent_ = y;
}


/* 插入新元素的逻辑定义 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::insert_new_node_impl(node_type* x) {
    x->left_ = nil_;
    x->right_ = nil_;
    x->is_black_ = false;
    if (root_ == nil_){
        root_ = x;
        x->parent_ = nil_;
        x->is_black_ = true;
        return;
    }
    node_type* cur = root_;
    while (true){
        if (compare_function_(x->data_, cur->data_)){
            if (cur->left_ == nil_) {
                cur->left_ = x;
                x->parent_ = cur;
                return;
            }
            cur = cur->left_;
        }
        else{
            if (cur->right_ == nil_){
                cur->right_ = x;
                x->parent_ = cur;
                return;
            }
            cur = cur->right_;
        }
    }
}

/* 插入新元素后纠正红黑树的逻辑定义 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::insert_fix_impl(node_type* x) noexcept{
    while (x != root_ && x->parent_->is_black_ == false) {
        node_type* y = x->parent_; /* 此时保证y是red */
        /* uncle 存在且为红色 */
        if (has_brother(y) && get_brother(y)->is_black_ == false) {
            y->is_black_ = get_brother(y)->is_black_ = true;
            y->parent_->is_black_ = false;
            x = y->parent_;
        }
        /* uncle不存在或者为黑色 */
        else {
            /* 单旋转场景 */
            if (is_left(x) && is_left(y)) {
                y->parent_->is_black_ = false;
                y->is_black_ = true;
                rotate_right(y->parent_); 
            }
            else if (is_right(x) && is_right(y)) {
                y->parent_->is_black_ = false;
                y->is_black_ = true;
                rotate_left(y->parent_);
            }
            /* 双旋转场景，第二次旋转在下一次循环中 */
            else if (is_right(x) && is_left(y)) {
                rotate_left(y);
                x = y;
            }
            else if (is_left(x) && is_right(y)) {
                rotate_right(y);
                x = y;
            }
        }
    }
    root_->is_black_ = true;
}

/* 插入新元素 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
template<typename V, typename>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::insert(V&& value){
    if constexpr (!IS_MULTI) {
        node_type* temp = find_node(value);
        if (temp) {
            if constexpr (IS_MAP) {
                temp->data_.second = mstl::forward<value_type> (value.second);
            }
            return;
        }
    }
    node_type* new_node = allocator_.allocate(1);
    new(new_node) node_type(mstl::forward<V>(value));
    insert_new_node_impl(new_node);
    insert_fix_impl(new_node);
    current_size_ ++;
}


/* 删除节点的逻辑实现 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>:: erase_node_impl(node_type* node) {
    /* node有两个节点，找到第一个后继的节点，问题转化为删除第一个后继节点node，转化后的node不可能有左孩子 */
    if (!isNil(node->left_) && !isNil(node->right_)) {
        node_type* temp = node->right_;
        while (!isNil(temp->left_)) {
            temp = temp->left_;
        }
        node->data_ = mstl::move(temp->data_);
        node = temp;
    }

    /* 保证最多只有左孩子 */
    if (node == root_) {
        if (!isNil(node->left_)) {
            root_ = node->left_;
            root_->parent_ = nil_;
        }
        node->~node_type();
        allocator_.deallocate(node, 1);
        node = nullptr;
        return;
    }

    /* node节点没有孩子 */
    if (isNil(node->left_) && isNil(node->right_)) {
        /* 被删除节点是红色，直接操作就可以 */
        if (node->is_black_ == false) {
            if (is_left(node)) {
                node->parent_->left_ = nil_;
            }
            else {
                node->parent_->right_ = nil_;
            }
            node->~node_type();
            allocator_.deallocate(node, 1);
            node = nullptr;
        }
        /* 最麻烦的情况，此时一定有bro，且bro或者bro的孩子为黑色 */
        else {
            node_type* bro = get_brother(node);
            /* bro是红色的，需要先将问题转化为node与黑色bro节点 */
            if (bro->is_black_ == false) {
                mstl::swap(bro->is_black_, bro->parent_->is_black_);
                if (is_left(bro)) {
                    rotate_right(bro->parent_);
                }
                else {
                    rotate_left(bro->parent_);
                }
                bro = get_brother(node);
            }
            /* 此时必定进入这一步，bro必定为黑色 */
            /* bro节点的不同节点方向(右)有一个红色节点 */
            if (is_left(bro) && !isNil(bro->right_) && bro->right_->is_black_ == false){
                mstl::swap(bro->is_black_, bro->right_->is_black_);
                rotate_left(bro);
            }
            /* bro节点的不同节点方向(左)有一个红色节点 */
            else if (is_right(bro) && !isNil(bro->left_) && bro->left_->is_black_ == false) {
                mstl::swap(bro->is_black_, bro->left_->is_black_);
                rotate_right(bro);
            }
            bro = get_brother(node);
            /* bro节点的相同节点方向(左)有一个红色节点 */
            if (is_left(bro) && !isNil(bro->left_) && bro->left_->is_black_ == false) {
                node->parent_->is_black_ = true;
                bro->left_->is_black_ = true;
                node->parent_->right_ = nil_;
                rotate_left(node->parent_);
                node->~node_type();
                allocator_.deallocate(node, 1);
                node = nullptr;
            }
            /* bro节点的相同节点方向(右)有一个红色节点 */
            else if (is_right(bro) && !isNil(bro->right_) && bro->right_->is_black_ == false) {
                node->parent_->is_black_ = true;
                bro->right_->is_black_ = true;
                node->parent_->left_ = nil_;
                rotate_right(node->parent_);
                node->~node_type();
                allocator_.deallocate(node, 1);
                node = nullptr;
            }
            /* bro无红色孩子节点 */
            else {
                node_type* par = node->parent_;
                if (is_left(node)) {
                    par->left_ = nil_;
                }
                else {
                    par->right_ = nil_;
                }
                node->~node_type();
                allocator_.deallocate(node, 1);
                node = nullptr;
                /* parent为红 */
                if (par->is_black_ == false) {
                    mstl::swap(par->is_black_, bro->is_black_);
                }
                else {
                    bro->is_black_ = false;
                    node = par;
                    while (node != root_) {
                        if (node->parent_->is_black_ == false) {
                            mstl::swap(node->parent_->is_black_, get_brother(node)->is_black_);
                            break;
                        }
                        else {
                            get_brother(node)->is_black_ = false;
                            node = node->parent_;
                        }
                    }
                    if (node == root_) {
                        node->is_black_ = true;
                    }
                }
            }
        }
    }
    /* node节点只有左孩子(说明node没有被转换过)，此时node必定是黑色，孩子必定是红色（否则不符合到nil节点长度相等定理） */
    else if (!isNil(node->left_)) {
        node->left_->parent_ = node->parent_;
        node->parent_->left_ = node->left_;
        node->left_->is_black_ = true;
        node->~node_type();
        allocator_.deallocate(node, 1);
    }
    /* node节点只有右孩子(可能是后继的右孩子，也可能是初始node的右孩子)，此时node必定是黑色，孩子必定是红色 */
    else {
        node->right_->parent_ = node->parent_;
        node->parent_->right_ = node->right_;
        node->right_->is_black_ = true;
        node->~node_type();
        allocator_.deallocate(node, 1);
    }
    node = nullptr;
}

/* 删除函数 */
template<typename T, typename U, typename Compare, typename Allocator, bool IS_MAP, bool IS_MULTI>
template<typename V, typename>
inline void RedBlackTree<T, U, Compare, Allocator, IS_MAP, IS_MULTI>::erase(const V& value){
    node_type* node = find_node(value);
    if (node != nullptr) {
        erase_node_impl(node);
        current_size_ --;
    }
}


END_NAMESPACE