#pragma once

#include <type_traits>   //std::conditional_t
#include "mstl_pair.h"


template<typename T, typename U, const bool IS_MAP = false>
struct RbtNode{
    using key_type    =   T;
    using value_type  =   U;
    using NodeType    =   std::conditional_t<IS_MAP, mstl::pair<const key_type, value_type>, key_type>;

    RbtNode*   left_;
    RbtNode*   right_;
    RbtNode*   parent_;
    NodeType   data_;
    bool       is_black_;

    explicit RbtNode(const key_type& key) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(key),
        is_black_(false)
    {}

    explicit RbtNode(key_type&& key) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(mstl::move(key)),
        is_black_(false)
    {}


    template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    RbtNode(const mstl::pair<key_type, value_type>& x) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(x),
        is_black_(false)
    {}

    template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    RbtNode(mstl::pair<key_type, value_type>&& x) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_(mstl::move(x)),
        is_black_(false)
    {}
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
    
    template<typename V>
    void insert(V&& value);

    template<bool B = IS_MAP, typename = std::enable_if_t<!B>>
    void erase(const key_type& key);

    template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    void erase(const mstl::pair<key_type, value_type>& value);


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

    bool is_left(node_type* x) const { return (x->parent_)->left_ == x; }
    bool is_right(node_type* x) const { return (x->parent_)->right_ == x; }
    bool has_brother(node_type* x) const { return is_left? x->parent_->right_ != nil_ : x->parent_->left_ != nil_; }
    node_type* get_brother(node_type* x) const { return is_left? x->parent_->right_ : x->parent_->left_;  }
    bool isNil(node_type* x) const { return x == nil_;}

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
template<typename V>
void RedBlackTree<Derived, T, U, IS_MAP>::insert(V&& value){
    node_type* new_node = allocator_.allocate(1);
    new(new_node) node_type(mstl::forward<V>(value));
    insert_new_node(new_node);
    insert_fix(new_node);
}
