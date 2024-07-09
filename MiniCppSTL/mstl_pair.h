#pragma once

#include "mstl_global.h"
#include <type_traits>      //remove_reference

NAMESPACE_MSTL

template<typename T, typename U>
class pair {
public:
    T first;
    U second;

    pair() : first(T()), second(U()) {}

    pair(const T& x, const U& y) : first(x), second(y) {}

    pair(const mstl::pair<T, U>& other) : first(other.first), second(other.second) {}

    pair& operator=(const mstl::pair<T, U>& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }

    template<typename V, typename W>
    friend bool operator ==(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);
    
    template<typename V, typename W>
    friend bool operator !=(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);

    template<typename V, typename W>
    friend bool operator <(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);

    template<typename V, typename W>
    friend bool operator >(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);

    template<typename V, typename W>
    friend bool operator <=(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);

    template<typename V, typename W>
    friend bool operator >=(const mstl::pair<V, W>& lhs, const mstl::pair<V, W>& rhs);
};


template<typename T, typename U>
inline bool operator ==(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return static_cast<bool>(lhs.first == rhs.second && lhs.second == rhs.second);
}

template<typename T, typename U>
inline bool operator !=(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return !(lhs == rhs);
}

template<typename T, typename U>
inline bool operator <(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return static_cast<bool>(lhs.first < rhs.first);
}

template<typename T, typename U>
inline bool operator >(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return static_cast<bool> (lhs.first > rhs.first);
}

template<typename T, typename U>
inline bool operator <=(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return !(lhs > rhs);
}

template<typename T, typename U>
inline bool operator >=(const mstl::pair<T, U>& lhs, const mstl::pair<T, U>& rhs){
    return !(lhs < rhs);
}

/* make pair! */
template<typename T, typename U>
inline mstl::pair<T, U> make_pair(const T& f, const U& s){
    return mstl::pair<T, U>{f, s};
}


END_NAMESPACE