#pragma once

#include <type_traits>   //std::conditional_t
#include "mstl_pair.h"


template<typename T, typename U, const bool IS_MAP>
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

    RbtNode(const key_type& key, const value_type& value) : 
        left_(nullptr), 
        right_(nullptr), 
        parent_(nullptr), 
        data_({key, value}),
        is_black_(false)
    {}
};



template<typename T, typename U = T, const bool IS_MAP = false>
class RedBlackTree{
public:
    using key_type = T;
    using value_type = U;
    using node_type = RbtNode<key_type, value_type, IS_MAP>;

    RedBlackTree() : root_(nullptr), nil_(new node_type(T())){}

    ~RedBlackTree(){
        delete nil_;
    }

    void insert(const key_type& key);

    template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    void insert(const mstl::pair<key_type, value_type>& value);

    void erase(const key_type& key);

    template<bool B = IS_MAP, typename = std::enable_if_t<B>>
    void erase(const mstl::pair<key_type, value_type>& value);


private:
    node_type* root_;
    node_type* nil_;

private:
    void leftRotate(node_type* x);

    void rightRotate(node_type* x);

    void leftDelete(node_type* x);

    void rightDelete(node_type* x);

    bool isLeftChild(node_type* x) const { return (x->parent_)->left_ == x; }
    bool isRightChild(node_type* x) const { return (x->parent_)->right == x; }
    bool isNil(node_type* x) const { return x == nil_;}

};

template<typename T, typename U, const bool IS_MAP>
void RedBlackTree<T, U, IS_MAP>::leftRotate(node_type* x){
    node_type* y = x->right_;
    x->right_ = y->left_;
    if (!isNil(y->left_)){
        y->left_->parent_ = x;
    }
    if (isNil(x->parent_)){
        root_ = y;
    }
    else if (isLeftChild(x)){
        x->parent_->left = y;
    }
    else{
        x->parent_->right = y;
    }
    y->parent_ = x->parent_;
    y->left_ = x;
    x->parent_ = y;
}

template<typename T, typename U, const bool IS_MAP>
void RedBlackTree<T, U, IS_MAP>::rightRotate(node_type* x){
    node_type* y = x->left_;
    x->left_ = y->right_;
    if (!isNil(y->right_)){
        y->right_->parent_ = x;
    }
    if (isNil(x->parent_)){
        root_ = y;
    }
    else if (isLeftChild(x)){
        x->parent_->left = y;
    }
    else{
        x->parent_->right = y;
    }
    y->parent_ = x->parent_;
    y->right_ = x;
    x->parent_ = y;
}

