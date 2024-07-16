#include "mstl_global.h"
#include "mstl_rbtree.h"
#include "mstl_allocator.h"
#include "mstl_iterator_base.h"


NAMESPACE_MSTL

//TODO 迭代器， 目前实现的好像是类似multiset，没有判重的逻辑
template<typename T, typename COMPARE_FUNCTION = mstl::less<T>, typename Allocator = mstl::Allocator<T>>
class set: public mstl::RedBlackTree<T, T, COMPARE_FUNCTION, Allocator, false> {
public:


    set() = default;

    virtual ~set() override = default;

    
private:

};


END_NAMESPACE