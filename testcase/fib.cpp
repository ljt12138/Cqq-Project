#include <iostream>
#include <cstdio>
using namespace std;

int ans = 0;

int fib(int n)
{
	if (n <= 1) ans = ans + 1;
	else {
		fib(n-1);
		fib(n-2);
	}
}

int main()
{
	fib(34);
	cout << ans << endl;
	return 0;
}
