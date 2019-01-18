#include <iostream>
#include <cstdio>
using namespace std;

int ans = 0;
int fib(int n)
{
	if (n <= 1) {
		ans = ans + 1;
		return 0;
	}
	fib(n-1);
	fib(n-2);
}

int main()
{
	fib(35);
	cout << ans << endl;
	return 0;
}
