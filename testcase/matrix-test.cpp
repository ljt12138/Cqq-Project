#include <iostream>
#include <cstdio>
using namespace std;

int a[500][500], b[500][500], c[500][500];

int main()
{
	int n;
	cin >> n;
	int x = 31, y = 57, z = 97, pre = 0;
	for (int i = 0; i < n; i = i + 1)
		for (int j = 0; j < n; j = j + 1) {
			pre = a[i][j] = (pre * x + y) % z;
			pre = b[i][j] = (pre * x + y) % z;
		}
	return 0;
}
