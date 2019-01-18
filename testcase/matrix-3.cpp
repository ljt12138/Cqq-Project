#include <iostream>
#include <cstdio>
using namespace std;

int a[500][500], b[500][500], c[500][500];

int main()
{
	int n;
	cin >> n;
	const int A = 1;
	int x, y, z, pre, i, j, k;
	pre = 0;
	x = 31; y = 57; z = 97; 
	for (i = 0; i < n; i = i + 1)
		for (j = 0; j < n; j = j + 1) {
			pre = a[i][j] = (pre * x + y) % z;
			pre = b[i][j] = (pre * x + y) % z;
		}
	for (i = 0; i < n; i = i + 1) {
		for (j = 0; j < n; j = j + 1) {
			for (k = 0; k < n; k = k + 1) {
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}
	for (i = 0; i < n; i = i + 1) {
		for (j = 0; j < n; j = j + 1)
			cout << c[i][j] * A;
		cout << endl;
	}
	return 0;
}
