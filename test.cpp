#include "containers/mstl_vector"
#include "containers/mstl_array"
#include "utility/mstl_pair"
#include "algorithms/mstl_algorithm"
#include "containers/mstl_deque"
#include "containers/mstl_set"
#include "containers/mstl_map"
#include "adapters/mstl_stack"
#include "adapters/mstl_queue"
#include "containers/mstl_list"
#include "adapters/mstl_priority_queue"
#include "containers/mstl_string"

#include <bits/stdc++.h>
using namespace std;


int main() {

    //vector
#if 0
{
    mstl::vector<int> my_vec_a(10, -1);
    mstl::vector<int> my_vec_b(10, -1);
    std::cout << (my_vec_a == my_vec_b) << std::endl;
    my_vec_b.push_back(2);
    for (int i = 0; i < my_vec_b.size(); ++i) {
        std::cout << my_vec_b[i] << " \n"[i == my_vec_b.size() - 1];
    }
    mstl::vector<char> my_vec_c{'1', '2', '?'};
    for (int i = 0; i < my_vec_c.size(); ++i) {
        cout << my_vec_c[i] << " \n"[i == my_vec_c.size() - 1];
    }
    for (auto it = my_vec_c.begin(); it != my_vec_c.end(); ++it) {
        *it = '!';
        cout << (*it) << " ";
    }
    for (auto& x : my_vec_c) {
        x = '~';
    }
    cout << endl;
    for (auto x : my_vec_c) {
        cout << x << endl;
    }
    auto it = my_vec_c.cbegin();
    //*it = 3;
    {   char c = 'g';
        for (auto& t : my_vec_c) {
            t = c ++;
        }
    }
    for (auto it = my_vec_c.rbegin(); it != my_vec_c.rend(); ++it) {
        cout << *it << ' ';
    }
    cout << endl;

    cout << *mstl::max_element(my_vec_c.begin(), my_vec_c.end()) << endl;
    cout << *mstl::min_element(my_vec_c.begin(), my_vec_c.end()) << endl;

    {
        auto it = my_vec_c.begin();
        auto rt = mstl::next(it);
        cout << *rt << endl;
    }
    {   
        mstl::vector<int> a(20);
        mstl::iota(a.begin(), a.end(), 22);
        for (auto x : a) {
            cout << x << ' ';
        }
    }
    {
        mstl::vector<mstl::pair<int, char>> a;
        a.emplace_back(2, 'c');
        a.emplace_back(4, '@');
        cout << endl;
        for (int i = 0; i < a.size(); ++i) {
            cout << a[i].first << " " << a[i].second << " \n"[i == a.size() - 1];
        }
        
    }

    /* erase test*/
    {   
        cout << "vector erase test: \n";
        mstl::vector<int> a(20);
        mstl::iota(a.begin(), a.end(), 0);
        {
            auto b = a;
            b.erase(b.begin() + 15);
            for (int i = 0; i < b.size(); ++i) {
                cout << b[i] << " \n"[i == b.size() - 1];
            }
        }
        {
            auto b = a;
            b.erase(b.begin() + 2, b.end());
            for (int i = 0; i < b.size(); ++i) {
                cout << b[i] << " \n"[i == b.size() - 1];
            }
            a.clear();
        }
    }
    /* insertion test */
    {
        cout << "vector inser test: \n";
        mstl::vector<int> a(10, -1);
        a.insert(a.begin(), 0);
        for (int i = 0; i < a.size(); ++i) {
            cout << a[i] << " \n"[i == a.size() - 1];
        }
        auto b = a;
        
        mstl::deque<int> c{a.begin(), a.end()};
        auto t = c.end() - c.begin();
        b.insert(b.begin() + 2, c.begin(), c.end());    /* 使用其他容器的迭代器构造! */
        for (int i = 0; i < b.size(); ++i) {
            cout << b[i] << " \n"[i == b.size() - 1];
        }
    }
    
}
#endif


//array
#if 0
{
    mstl::array<int, 23> t {1,2,3};
   // t[0] = 5;
    for (int i = 0; i < t.size(); ++i) {
        cout << t[i] << " \n"[i == t.size() - 1];
    }

    for (auto& x : t) {
        cout << x << ' ';
    }
    cout << endl;
    for (const auto& x : t) {
        cout << x << ' ';
    }
    cout << endl;
    mstl::vector<int> a;
    {int cnt = 0;for (auto&x : t) {
        x = cnt ++;
    }}
    for (auto it = t.crbegin(); it != t.crend(); ++it) {
        cout << *it << ' ';
    }
    cout << endl;
}
#endif

//pair
#if 0
{
    mstl::pair<int, int> a_p{2, 2};
    mstl::pair<int, int> ba_p{2, 2};
    cout << ( a_p <= ba_p);
}
#endif
#if 0
    {
        mstl::vector<int> a((int)1e2);
        int t = 1;
        for (auto it = a.rbegin(); it != a.rend(); ++it) {
            *it = t++;
        }
        mstl::sort(a.begin(), a.end());
        for (int i = 0; i < a.size(); ++i) {
            cout << a[i] << ' ';
        }
    }
#endif

//deque
#if 0
    {
        mstl::deque<int> a{1,2,3,4,5};
        //cout << a.size() << endl;
        cout << "capacity" << a.capacity() << endl;
        for (auto it = a.rbegin(); it != a.rend(); ++it) {
            cout << (*it) << '\n';
        }

        mstl::deque<int> b{2,3,34,12,31,23,12,31,23,12,312,3,1};
        a = mstl::move(b);
        int t = -2;
        for (auto& x : a) {
            x = t --;
        }
        for (auto it = a.cbegin(); it != a.cend(); ++it) {
            cout << (*it) << endl;
        }
        mstl::deque<int> c(a.begin(), a.end());
        cout << c.size() << endl;
        for (int i = 0; i < c.size(); ++i) {
            cout << c[i] << " \n"[i == c.size() - 1];
        }
        mstl::deque<mstl::pair<int, int>> d;
        d.emplace_back(2,3);
        d.emplace_front(0, 0);
        d.emplace_back(4,4);
        d.emplace_back({22,4});
        for (auto&[x, y] : d) {
            cout << x << " " << y << endl;
        }
        a == b;
        for (int i = 10; i >= 1; --i) {
            d.emplace_back(i, i - 2);
        }
        cout << "--------------------" << endl;
        mstl::sort(d.begin(), d.end());
        for (auto&[x, y] : d) {
            cout << x << ' ' << y << "   ";
        }
        cout << endl;
        auto it = mstl::upper_bound(d.begin(), d.end(), mstl::pair{3, 1});
        while (it != d.end()) {
            cout << (it->first) << " " << (it->second) << "   ";
            ++it;
        }
            /* erase test*/
        {
        cout << "Deque erase test: \n" ;
        mstl::deque<int> a(20);
        mstl::iota(a.begin(), a.end(), 0);
        {
            auto b = a;
            b.erase(b.begin() + 15);
            for (int i = 0; i < b.size(); ++i) {
                cout << b[i] << " \n"[i == b.size() - 1];
            }
            }   
            {
                auto b = a;
                b.erase(b.begin() + 2, b.end());
                for (int i = 0; i < b.size(); ++i) {
                    cout << b[i] << " \n"[i == b.size() - 1];
                }
                a.clear();
            }
        }
        
    }
        /* insertion test */
    {
        cout << "deque inser test: \n";
        mstl::deque<int> a(10, -1);
        a.insert(a.begin(), 0);
        auto t = a.end() - a.begin();
        for (int i = 0; i < a.size(); ++i) {
            cout << a[i] << " \n"[i == a.size() - 1];
        }
        auto b = a;
        mstl::vector<int> c{a.begin(), a.end()};
         t = c.end() - c.begin();
        b.insert(b.begin() + 2, c.begin(), c.end());    /* 使用其他容器的迭代器构造! */
        for (int i = 0; i < b.size(); ++i) {
            cout << b[i] << " \n"[i == b.size() - 1];
        }
    }
#endif


//红黑树测试，好像测试不了，得先继承才行。。
#if 0
    {   
        mstl::multiset<int> s;
        s.insert(2);
        s.insert(4);
        for (int i = 10; i >= -2; --i) {
            s.insert(i);
            s.insert(i);
        }
        for (auto it = s.rbegin(); it != s.rend(); ++it) {
            cout << (*it) << " ";
        }
        cout << endl;
        {
            auto it = s.begin();
            it = next(it);
            cout << (*it) << endl;
        }
        cout <<"multiset count  2 : " << s.count(2) << endl;
        {
            auto it = s.lower_bound(2);
            cout << "lower_bound set" << (*it) << endl;
            auto rt = s.upper_bound(2);
            cout << "upper_bound set" << (*rt) << endl;
        }
        cout << s.size() << endl;
        for (auto it = s.cbegin(); it != s.cend(); ++it) {
            cout << (*it) << " \n"[mstl::next(it) == s.cend()];
        }
        s.erase(-2);
        s.erase(10);
        for (auto it = s.cbegin(); it != s.cend(); ++it) {
            cout << (*it) << " \n"[mstl::next(it) == s.cend()];
        }
        s.erase(5);
        s.erase(5);
        for (auto it = s.cbegin(); it != s.cend(); ++it) {
            cout << (*it) << " \n"[mstl::next(it) == s.cend()];
        }
        auto it = s.begin();
        s.erase(it);
        for (auto it = s.cbegin(); it != s.cend(); ++it) {
            cout << (*it) << " \n"[mstl::next(it) == s.cend()];
        }
        it = s.end();
        --it;
        cout << (*it) << endl;
        {
            mstl::set<int> t{1,2,3};
            for (auto it = t.begin(); it != t.end(); ++it) {
                cout << (*it) << ' ';
            }
            mstl::set<int> a;
            a = t;
        }
    }
#endif

#if 0
    {
        mstl::map<int, int> mapp;
        mapp.insert(mstl::pair{2, 3});
        for (int i = 110; i >99; --i ) {
            mapp.insert(mstl::pair{i, -i});
        }
        for (auto&[x, y]: mapp) {
            cout << x << " " << y <<endl; y = -1;
        }
        for (auto&[x, y]: mapp) {
            cout << x << " " << y <<endl; 
        }
        cout << "map count : " << mapp.count(mstl::pair{2, -1}) << endl;
        auto it = mapp.begin();
        cout << (it->first) << endl;
        auto tt = mapp.lower_bound(mstl::make_pair(2, -1));
        cout << "mapp lowerbound : " << (tt->first) << " " << tt->second << endl;
        auto rr = mapp.upper_bound(mstl::make_pair(2, -1));
        cout << "mapp upperbound : " << (rr->first) << " " << rr->second << endl;

            it = mapp.begin(); 
            cout << it->first << " " << it->second << endl;
            cout << "sz" << mapp.size() << endl;
        for (auto&[x, y]: mapp) {
            cout << x << " " << y <<' '; 
        }
        cout << endl;
        
        for (auto&[x, y]: mapp) {
            cout << x << " " << y <<' '; 
        }
        cout << endl;
        mapp.erase(it);
        for (auto&[x, y]: mapp) {
             cout << x << " " << y <<' '; 
         }

        mapp.clear();
        cout << mapp.size() << endl;
    }
#endif


//stack

#if 0
    {
        mstl::stack<int> a{2,3,4,5,61};
        cout << a.size() << endl;
        while (!a.empty()) {
            cout<< a.top() << ' ';
            a.pop();
        }
        cout << endl;
    }
#endif


//list
#if 0
    {   
        std::cout << "list: initializer list test\n";
        mstl::list<int> a{2,3,4,5,6};
        for (auto it = a.cbegin(); it != a.cend(); ++it) {
            cout << (*it) << ' ';
        }
        cout << endl;
        for (auto it = a.rbegin(); it != a.rend(); ++it) {
            cout << (*it) << ' ';
        }
        cout << endl;
        {
            std::cout << "list: set iterator list test\n";
            mstl::set<int> b{3,2,4,5,1};
             mstl::list<int> c{b.begin(), b.end()};
            for (auto x : c) {
                cout << x << ' ';
            }
            cout << endl;
        }
        {
            cout << "list insert & erase test\n";
            mstl::list<int> a{2,3,4,5,6};
            a.insert((a.begin()), 1);
            for (auto t : a) {
                cout << t << ' ';
            }
            cout << endl;
            mstl::multiset<int>s{8,8,8,8,8};
            a.insert(a.end(), s.begin(), s.end());
            for (auto t : a) {
                cout << t << ' ';
            }
            cout << endl;
            a.erase(a.begin());
            for (auto t : a) {
                cout << t << ' ';
            }
            cout << endl;
            a.erase(a.begin(), --a.end());
            for (auto t : a) {
                cout << t << ' ';
            }
            cout << endl;
            mstl::list<int> b{a.begin(), a.end()};
            cout << (b == a ? "YES" : "NO") << endl;
        }
        {
            mstl::vector<int> b{1,1,1};
            mstl::list<int> a{2,3,4};
            a.insert(a.begin(), b.begin(), b.end());
            for (auto t : a) { cout << t << ' ';} cout << endl;
            a.reverse();
            for (auto t : a) { cout << t << ' ';} cout << endl;
            for (auto t : a) { cout << t << ' ';} cout << endl;
            a.sort([](const int& a, const int& b) {
                return a >  b;
            });
            for (auto t : a) { cout << t << ' ';} cout << endl;
        }

    }
#endif


// priority_queue
#if 0
    {
        mstl::priority_queue<int, std::vector<int>, std::less<int>> pq{1,2,3};
        cout << pq.size() << endl;
        while (!pq.empty()){
            cout << pq.top() << ' ';
            pq.pop();
        }
        cout << endl;
    }

    {   
        mstl::array<int, 4> s{2,3,4,5};
        mstl::priority_queue<int, std::vector<int>, mstl::greater<int>> pq {s.begin(), s.end()};
        pq.push(-1);
        pq.push(-2);
        pq.emplace(333);
        while (!pq.empty()){
            cout << pq.top() << ' ';
            pq.pop();
        }
        cout << endl;
    }
    {
        mstl::priority_queue<mstl::pair<int, int>, std::vector<mstl::pair<int, int>>, mstl::greater<mstl::pair<int, int>>> pq;
        pq.emplace(2,3);
        pq.push(mstl::pair<int, int>{2,4});
        pq.emplace(mstl::pair<int, int>{2,5});
        while (!pq.empty()){
            cout << pq.top().first << " " << pq.top().second << '\n';
            pq.pop();
        }
        cout << endl;
    }
#endif
    
    

    return 0;
}