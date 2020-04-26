#include <iostream>
#include <map>

using namespace std;

int fib(int n)
{
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

int fastFib(int n, map<int, int> &memo)
{
    if (n == 0 || n == 1)
        return 1;

    try {
        return memo.at(n);
    } catch(out_of_range) {
        int result = fastFib(n - 1, memo) + fastFib(n - 2, memo);
        memo[n] = result;
        return result;
    }
}

int fastFib(int n)
{
    map<int, int> memo;
    return fastFib(n, memo);
}

int main()
{
    for (int i = 0; i <= 121; i++) {
        // cout << "fib(" << i << ") = " << fib(i) << endl;
        cout << "fib(" << i << ") = " << fastFib(i) << endl;
    }
}
