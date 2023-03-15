#include <iostream>
#include <algorithm>
#include <vector>
#include "buffer.h"

using namespace std;

void PrintBuffer(Buffer<int>& buff) {
    for (int elem : buff) {
        cout << elem << " ";
    }
    cout << endl;
}

void PrintBuffer(Buffer<string>& buff) {
    for (string elem : buff) {
        cout << elem << " ";
    }
    cout << endl;
}

void PrintBuffer(Buffer<vector<int>>& buff) {
    for (auto vec : buff) {
        for (auto elem : vec) 
            cout << elem << " ";
        cout << endl;
    }
}

int main() {

    Buffer<int> b;
    b.PushBack(3);      // 3 *
    b.PushBack(4);      // 3 4 * *
    PrintBuffer(b);     // "3 4"
    b.PushBack(5);      // 3 4 5 *
    b.PopFront();       // * 4 5 *
    PrintBuffer(b);     // "4 5"
    
    b.PushFront(7);     // 7 4 5 *
    PrintBuffer(b);     // "7 4 5"
    b.PushFront(8);     // 7 4 5 * * * * 8
    b.PushFront(9);     // 7 4 5 * * * 9 8
    PrintBuffer(b);     // "9 8 7 4 5"
    sort(b.begin(), 
            b.end());   // 7 8 9 * * * 4 5
    //sort(b.cbegin(), b.cend()); -> assignment of read-only location
    PrintBuffer(b);     // "4 5 7 8 9"

    b.PopBack();        // 7 8 * * * * 4 5
    b.PopBack();        // 7 * * * * * 4 5
    b.PushFront(9);     // 7 * * * * 9 4 5
    b.PopBack();        // * * * * * 9 4 5
    PrintBuffer(b);     // "9 4 5"
    reverse(b.begin(),
        b.end());       // * * * * * 5 4 9
    PrintBuffer(b);     // "5 4 9"

    Buffer<vector<int>> bvi;
    bvi.EmplaceBack(5,1);
    bvi.EmplaceBack(3);
    PrintBuffer(bvi);

    Buffer<string> s;
    string k = "lorem";
    s.PushBack(k);
    s.PushBack("ipsum");
    s.PushBack("foo");
    s.PushBack("bar");
    PrintBuffer(s);

    
    return 0;
}