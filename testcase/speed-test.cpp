#include <iostream>
#include <cstdio>
using namespace std;

const int n = 10000;
int a[2 * n + 1];
int b[2 * n + 1];

int main()
{
	int ans = 0, x = 10, y = 20;
	for (int i = 0; i < n; i = i + 1) {
		a[i] = i;
		b[i] = n-i;
	}
	for (int i = 0; i < n; i = i + 1) {
		int c[n + 1];
		for (int j = 0; j < n; j = j + 1) {
			ans = ans + a[j] * (b[i] + 2 + 3 * b[i + i]) * i;
			a[i] = 1;
			c[i] = 1;
		}
	}
	cout << ans << endl;
	return 0;
}
