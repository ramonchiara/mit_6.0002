#include <gmpxx.h>
#include <iostream>
#include <map>

using namespace std;

int fib(mpz_class n)
{
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

mpz_class fastFib(mpz_class n, map<mpz_class, mpz_class> &memo)
{
    if (n == 0 || n == 1)
        return 1;

    try {
        return memo.at(n);
    } catch(out_of_range&) {
        mpz_class result = fastFib(n - 1, memo) + fastFib(n - 2, memo);
        memo[n] = result;
        return result;
    }
}

mpz_class fastFib(mpz_class n)
{
    map<mpz_class, mpz_class> memo;
    return fastFib(n, memo);
}

int main()
{
    for (int i = 0; i <= 121; i++) {
        // cout << "fib(" << i << ") = " << fib(i) << endl;
        cout << "fib(" << i << ") = " << fastFib(i).get_str() << endl;
    }
}
