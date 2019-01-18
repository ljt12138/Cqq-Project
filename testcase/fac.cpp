#include <iostream>
#include <cstdio>
using namespace std;

const int mod = 1000000007;

int ans = 1;

int fac(int n)
{
	if (n > 0) {
		ans = ans * n % mod;
		fac(n-1);
	}
}

int main()
{
	fac(100000);
	cout << ans << endl;
	return 0;
}
