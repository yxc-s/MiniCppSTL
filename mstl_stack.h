#include "mstl_global.h"
#include "mstl_deque.h"
#include "mstl_vector.h"


NAMESPACE_MSTL

/* stack不需要push_front，所以这里直接将deque中的块起始下标从0开始 */
template<typename T, typename Container = mstl::deque<T, mstl::Allocator<T>, 512, 0>>
class stack{
    using this_stack_type = stack<T, Container>;
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = typename Container::size_type;

    stack() = default;

    stack(const this_stack_type& other) = default;
    
    stack (this_stack_type&& other) : container_(mstl::move(other.container_)) {}

    stack(const std::initializer_list<T>& init) {
        for (const auto& value : init) {
            container_.push_back(value);
        }
    }

    ~stack() = default;

    template<typename U, typename = std::enable_if_t<std::is_same_v<std::remove_reference_t<U>, T>>>
    void push(U&& value) {
        container_.push_back(mstl::forward<U>(value));
    }

    void pop(){
        container_.pop_back();
    }

    reference top() { return container_.back(); }
    const_reference top() const { return container_.back(); }

    this_stack_type& operator =(const this_stack_type& other) {
        if (this != &other) {
            container_ = other.container_;
        }
        return *this;
    }

    this_stack_type& operator =(this_stack_type&& other) noexcept {
        if (this != &other) {
            container_ = mstl::move(other.container_);
        }
        return *this;
    }
    
    size_type size() const { return container_.size(); }
    bool empty() const { return container_.empty(); }
    size_type capacity() const { return container_.capacity(); }

    template<typename U>
    friend bool operator ==(const mstl::stack<U>& lhs, const mstl::stack<U>& rhs);

    template<typename U>
    friend bool operator !=(const mstl::stack<U>& lhs, const mstl::stack<U>& rhs);

private:
    Container container_;


};


template<typename U>
bool operator ==(const mstl::stack<U>& lhs, const mstl::stack<U>& rhs) {
    return lhs.container_ == rhs.container_;
}

template<typename U>
bool operator !=(const mstl::stack<U>& lhs, const mstl::stack<U>& rhs) {
    return !(lhs == rhs);
}

END_NAMESPACE