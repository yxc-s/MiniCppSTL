#include "mstl_vector.h"
#include "mstl_array.h"
#include <iostream>
#include <array>
#include <assert.h>
#include <vector>
#include "mstl_pair.h"
using namespace std;
int main(){

    //vector
#if 0
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
#endif


//array
#if 0
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
    vector<int> a;
    {int cnt = 0;for (auto&x : t){
        x = cnt ++;
    }}
    for (auto it = t.crbegin(); it != t.crend(); ++it){
        cout << *it << ' ';
    }
    cout << endl;

#endif

//pair
#if 0
    mstl::pair<int, int> a{2, 2};
    mstl::pair<int, int> b{2, 2};
    cout << ( a <= b);
#endif

    

    return 0;
}