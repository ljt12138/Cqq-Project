#include <iostream>
#include <cstdio>
using namespace std;

int fib(int i)
{
	if (i <= 1) return 1;
	return fib(i-1) + fib(i-2);
}

int main()
{
	const int n = 35;
	cout << fib(n) << endl;
	return 0;
}
