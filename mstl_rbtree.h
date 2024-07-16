#pragma once

#include <type_traits>   //std::conditional_t
#include "mstl_pair.h"


template<typename T, typename U, const bool IS_MAP = false>
struct RbtNode{
    using data_type    =   std::conditional_t<IS_MAP, mstl::pair<T, U>, T>;

    RbtNode*   left_;
    RbtNode*   right_;
    RbtNode*   parent_;
    data_type  data_;
    bool       is_black_;

    explicit RbtNode(const data_type& value) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(value),
        is_black_(false)
    {}

    explicit RbtNode(data_type&& value) : 
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


};



template<typename Derived, typename T, typename U, const bool IS_MAP = false>
class RedBlackTree{
public:
    using key_type = T;
    using value_type = U;
    using node_type = RbtNode<key_type, value_type, IS_MAP>;
    using compare_function_type = typename Derived::compare_functin;
    using allocator_type = typename Derived::allocator;



    RedBlackTree() : root_(nullptr), nil_(new node_type(T())){}

    ~RedBlackTree(){
        delete nil_;
    }

    // template<bool B = IS_MAP, typename = std::enable_if_t<!B>>
    // void insert(const key_type& key);

    // template<bool B = IS_MAP, typename = std::enable_if_t<!B>>
    // void insert(key_type&& key);

    // template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    // void insert(const mstl::pair<key_type, value_type>& value);

    // template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    // void insert(mstl::pair<key_type, value_type>&& value);
    
    template<typename V, typename = std::enable_if_t<std::is_same_v<V, typename node_type::data_type>>>
    void insert(V&& value);

    template<typename V, typename = std::enable_if_t<std::is_same_v<V, typename node_type::data_type>>>
    void erase(const V& value);


private:
    compare_function_type compare_function_;
    allocator_type allocator_;
    node_type* root_;
    node_type* nil_;

private:
    void rotate_left(node_type* x);

    void rotate_right(node_type* x);

    void delete_left(node_type* x);

    void delete_right(node_type* x);

    bool is_left(node_type* x) const noexcept { return (x->parent_)->left_ == x; }
    bool is_right(node_type* x) const noexcept { return (x->parent_)->right_ == x; }
    bool has_brother(node_type* x) const noexcept { return is_left(x) ? x->parent_->right_ != nil_ : x->parent_->left_ != nil_; }
    node_type* get_brother(node_type* x) const noexcept { return is_left(x) ? x->parent_->right_ : x->parent_->left_;  }
    bool isNil(node_type* x) const noexcept { return x == nil_;}

    void insert_new_node(node_type* x) noexcept{
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

    void insert_fix(node_type* x) noexcept{
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

    void erase_node(node_type* node) {
        /* node有两个节点，找到第一个后继的节点，问题转化为删除第一个后继节点node */
        if (!isNil(node->left_) && !isNil(node->right_)) {
            node_type* temp = node->right_;
            while (!isNil(temp->left_)) {
                temp = temp->left_;
            }
            node->data_ = mstl::move(temp->data_);
            node = temp;
        }
        /* node节点没有孩子 */
        if (isNil(node->left_) && isNil(node->right_)) {
            if (node->is_black_ == false) {
                if (is_left(node)) {
                    node->parent_->left_ = nil_;
                }
                else {
                    node->parent_->right = nil_;
                }
                node->~node_type();
                allocator_.deallocate(node, 1);
                node = nullptr;
            }
            /* 被删节点为黑色，根据红黑树距离长度的性质（必定有brother）*/
            else {
                node_type* bro = get_brother(node);
                /* bro节点为黑*/
                if (bro->is_black_) {
                    /* bro节点的不同节点方向(右)有一个红色节点 */
                    if (is_left(bro) && !isNil(bro->right_) && bro->right_->is_black_ == false){
                        rotate_left(bro);
                    }
                    /* bro节点的不同节点方向(左)有一个红色节点 */
                    else if (is_right(bro) && !isNil(bro->left_) && bro->left_->is_black_ == false) {
                        rotate_right(bro);
                    }
                    bro = get_brother(node);
                    /* bro节点的相同节点方向(左)有一个红色节点 */
                    if (is_left(bro) && !isNil(bro->left_) && bro->left_->is_black_ == false) {
                        node->parent_->is_black_ = true;
                        bro->left_->is_black_ = true;
                        node>parent_->right_ = nil_;
                        rotate_left(node->parent_);
                        node->~node_type();
                        allocator_.deallocate(node, 1);
                        node = nullptr;
                    }
                    /* bro节点的相同节点方向(右)有一个红色节点 */
                    else if (is_right(bro) && !isNil(bro->right_) && bro->right_->is_black_ == false) {
                        node->parent_->is_black_ = true;
                        bro->right_->is_black_ = true;
                        node>parent_->left_ = nil_;
                        rotate_right(node->parent_);
                        node->~node_type();
                        allocator_.deallocate(node, 1);
                        node = nullptr;
                    }
                    /* bro无红色孩子节点 */
                    else {
                        /* parent为红 */
                        if (node->parent_->is_black_ == false) {
                            mstl::swap(node->parent_->is_black_, bro->is_black_);
                        }
                        else {  //这个比较麻烦

                        }
                    }
                }
                /* 此时bro节点为红， parent必然为黑，而且bro必定有黑色的孩子 */ 
                else {
                    
                }
            }
        }
        /* node节点只有左孩子，此时node必定是黑色，孩子必定是红色 */
        else if (!isNil(node->left_)) {
            if (node->)
            node->left_->parent_ = node->parent_;
            node->left_->is_black_ = true;
            node->~node_type();
            allocator_.deallocate(node, 1);
        }
        /* node节点只有右孩子，此时node必定是黑色，孩子必定是红色 */
        else {
            node->right_->parent_ = node->parent_;
            node->right_->is_black_ = true;
            node->~node_type();
            allocator_.deallocate(node, 1);
        }
        node = nullptr;
    }
};

/* 左旋：右孩子y变为当前节点x的父亲节点，x成为y的左孩子，y之前的左孩子成为x的右孩子 */
template<typename Derived, typename T, typename U, const bool IS_MAP>
void RedBlackTree<Derived, T, U, IS_MAP>::rotate_left(node_type* x){
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
template<typename Derived, typename T, typename U, const bool IS_MAP>
void RedBlackTree<Derived, T, U, IS_MAP>::rotate_right(node_type* x){
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

/* 插入函数，SFINAE技术直接1换4! */
template<typename Derived, typename T, typename U, const bool IS_MAP>
template<typename V, typename>
void RedBlackTree<Derived, T, U, IS_MAP>::insert(V&& value){
    node_type* new_node = allocator_.allocate(1);
    new(new_node) node_type(mstl::forward<V>(value));
    insert_new_node(new_node);
    insert_fix(new_node);
}

/* 删除函数 */
template<typename Derived, typename T, typename U, const bool IS_MAP>
template<typename V, typename>
void RedBlackTree<Derived, T, U, IS_MAP>::erase(const V& value){
    node_type* node = find_node(value);
    if (node != nullptr) {
        erase_node(value);
        erase_fix(value);
    }
}

