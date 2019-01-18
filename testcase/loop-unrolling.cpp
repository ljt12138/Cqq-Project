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
			for (j = 0; j < n; j = j + 2) {
				c[i][j] = c[i][j] + b[k][j] * a[i][k];
				c[i][j+1] = c[i][j+1] + b[k][j+1] * a[i][k];
			}
		}
	}
	for (i = 0; i < n; i = i + 1) {
		for (j = 0; j < n; j = j + 1)
			cout << c[i][j];
		cout << endl;
	}
	return 0;
}

