#include <iostream>
#include <cstdio>
using namespace std;

const int n = 1000000;
int a[n+1], tmp[n+1];

int quick_sort(int i, int j)
{
	if (i >= j) return 0;
	int l, r, x;
	l = i; r = j;
	x = a[(i + j) / 2];
	while (i <= j) {
		while (a[i] < x) i = i + 1;
		while (a[j] > x) j = j - 1;
		if (i <= j) {
			int t = a[i];
			a[i] = a[j];
			a[j] = t;
			i = i + 1;
			j = j - 1;
		}
	}
	quick_sort(i, r);
	quick_sort(l, j);
}

int main()
{
	int x = 31, y = 123, z = 1000000009;
	for (int i = 1; i <= n; i = i + 1)
		a[i] = (x * a[i-1] + y) % z;
	quick_sort(1, n);
	for (int i = 1; i <= n; i = i + 1)
		cout << a[i];
	cout << endl;
	return 0;
}
