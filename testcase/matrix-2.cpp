#include <iostream>
#include <cstdio>
using namespace std;

int a[500][500], b[500][500], c[500][500];

int main()
{
	int n;
	cin >> n;
	int x, y, z, pre, i, j, k;
	pre = 0;
	x = 31; y = 57; z = 97; 
	for (i = 0; i < n; i = i + 1)
		for (j = 0; j < n; j = j + 1) {
			pre = a[i][j] = (pre * x + y) % z;
			pre = b[i][j] = (pre * x + y) % z;
		}
	for (i = 0; i < n; i = i + 1) {
		for (k = 0; k < n; k = k + 1) {
			for (j = 0; j < n; j = j + 1) {
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}
	int ans;
	ans = 0;
	for (i = 0; i < n; i = i + 1)
		for (j = 0; j < n; j = j + 1)
			ans = ans + c[i][j];
	cout << ans << endl;
	return 0;
}
