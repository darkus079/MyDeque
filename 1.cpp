#include "Deque.h"
#include <iostream>
#include <ctime>
#include <deque>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

int main() {
    Deque<int> d;

    for (int i = 1; i <= 10; ++i) {
        d.push_back(i);
    }

    for (int elem : d) {
        cout << elem << ' ';
    }

    cout << '\n';

    Deque<int>::iterator it = d.begin();
    cout << *it << '\n';
    it += 3;
    cout << *it << '\n';

    Deque<int>::reverse_iterator rit = d.rbegin();
    cout << *rit << '\n';

    vector<int> orv(10);
    vector<int>::const_iterator orvit = orv.cbegin();
    orvit = orv.begin();

    Deque<int>::const_iterator cit = d.begin();

    for (Deque<int>::reverse_iterator it = d.rbegin(); it != d.rend(); ++it) {
        cout << *it << ' ';
    }

    cout << '\n';

    Deque<Deque<int>> d2(3, Deque<int>(5, 1));

    d2[0].push_back(3);
    d2[0].push_back(4);
    d2[0].push_back(5);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < d2[i].size(); ++j) {
            cout << d2[i][j] << ' ';
        }

        cout << '\n';
    }
}