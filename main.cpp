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
//int primjer
//string primjer
//kompleksna struktura koja ima pointer primjer

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
    PrintBuffer(b);     // "4 5 7 8 9"

    b.PopBack();        // 7 8 * * * * 4 5
    b.PopBack();        // 7 * * * * * 4 5
    b.PushFront(9);     // 7 * * * * 9 4 5
    b.PopBack();        // * * * * * 9 4 5
    PrintBuffer(b);     // "9 4 5"
    reverse(b.begin(),
        b.end());       // * * * * * 9 4 5
    PrintBuffer(b);     // "5 4 9"

    Buffer<string> s;
    string k = "kifla";
    s.PushBack(k);
    s.PushBack("Marin");
    s.PushBack("jede");
    s.PushBack("kifle");
    s.PopFront();
    PrintBuffer(s);

    return 0;
}