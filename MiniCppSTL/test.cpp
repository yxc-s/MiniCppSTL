#include "mstl_vector.h"
#include <iostream>


int main(){

    mstl::vector<int> a(10, -1);
    mstl::vector<int> b(10, -1);
    b.fill(-2);
    std::cout << (a == b) << std::endl;
    b.push_back(2);
    for (int i = 0; i < b.size(); ++i){
        std::cout << b[i] << " \n"[i == b.size() - 1];
    }
    return 0;
}