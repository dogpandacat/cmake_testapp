//
// Created by AndyCui on 2023/10/12.
//

#include <iostream>
#include "print_collection_utils.h"

using namespace std;

template <typename T>
void print_elem(const T& t) {
    cout << "(" << t << ") ";
}

template <typename T>
void print_collection(const T& t) {
    cout << "  " << t.size() << " elements: ";

    for (const auto& p : t) {
        print_elem(p);
    }
    cout << endl;
}