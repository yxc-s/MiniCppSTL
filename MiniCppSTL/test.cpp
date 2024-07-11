#include <iostream>
#include "mstl_vector.h"
#include "mstl_array.h"

#include "mstl_pair.h"
#include "mstl_algorithm.h"
#include <bits/stdc++.h>




using namespace std;
int main(){

    //vector
#if 1
{
    mstl::vector<int> my_vec_a(10, -1);
    mstl::vector<int> my_vec_b(10, -1);
    my_vec_b.fill(-2);
    std::cout << (my_vec_a == my_vec_b) << std::endl;
    my_vec_b.push_back(2);
    for (int i = 0; i < my_vec_b.size(); ++i){
        std::cout << my_vec_b[i] << " \n"[i == my_vec_b.size() - 1];
    }
    mstl::vector<char> my_vec_c{'1', '2', '?'};
    for (int i = 0; i < my_vec_c.size(); ++i){
        cout << my_vec_c[i] << " \n"[i == my_vec_c.size() - 1];
    }
    for (auto it = my_vec_c.begin(); it != my_vec_c.end(); ++it){
        *it = '!';
        cout << (*it) << " ";
    }
    for (auto& x : my_vec_c){
        x = '~';
    }
    cout << endl;
    for (auto x : my_vec_c){
        cout << x << endl;
    }
    auto it = my_vec_c.cbegin();
    //*it = 3;
    {   char c = 'g';
        for (auto& t : my_vec_c){
            t = c ++;
        }
    }
    for (auto it = my_vec_c.rbegin(); it != my_vec_c.rend(); ++it){
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
        for (auto x : a){
            cout << x << ' ';
        }
    }
    
}
#endif


//array
#if 0
{
    mstl::array<int, 23> t {1,2,3};
   // t[0] = 5;
    for (int i = 0; i < t.size(); ++i){
        cout << t[i] << " \n"[i == t.size() - 1];
    }

    for (auto& x : t){
        cout << x << ' ';
    }
    cout << endl;
    for (const auto& x : t){
        cout << x << ' ';
    }
    cout << endl;
    mstl::vector<int> a;
    {int cnt = 0;for (auto&x : t){
        x = cnt ++;
    }}
    for (auto it = t.crbegin(); it != t.crend(); ++it){
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
        mstl::vector<int> a((int)1e5);
        int t = 1;
        for (auto it = a.rbegin(); it != a.rend(); ++it){
            *it = t++;
        }
        mstl::sort(a.begin(), a.end());
        for (int i = 0; i < a.size(); ++i){
            cout << a[i] << ' ';
        }
    }
#endif
    

    return 0;
}