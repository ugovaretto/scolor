#include <tuple>
#include <iostream>

using namespace std;

tuple< const int& > Get() {
    static const int i = 9;
    return tuple<const int&>(i);
}

int main(int, char**) {
    tuple< const int& > t(Get());
    cout << get<0>(t) << endl;
    return 0;
}
