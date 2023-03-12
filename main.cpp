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
    b.PushBack(3);
    b.PushBack(4);
    b.PushBack(5);
    PrintBuffer(b);

    b.PopFront();
    b.PopFront();
    b.PushBack(6);
    PrintBuffer(b);
    b.PopFront();
    b.PushBack(1);
    b.PushBack(2);
    PrintBuffer(b);
    sort(b.begin(), b.end());
    PrintBuffer(b);

    Buffer<string> s;
    s.PushBack("Marin");
    s.PushBack("voli");
    s.PushBack("Mireo");
    PrintBuffer(s);

    return 0;
}