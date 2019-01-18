#include <iostream>
#include <cstdio>
using namespace std;

int a[10000000];
const int n = 10;
const int m = n + 10, t = n + m;
int l;

int main()
{
	const int k = n + m + 1;
	int r;
	l = n + m;
	r = l + 1;
	cout << n << endl;
	cout << m << endl;
	cout << t << endl;
	cout << k << endl;
	cout << l << endl;
	cout << r << endl;
	return 0;
}
