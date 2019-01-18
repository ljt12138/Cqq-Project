#include <iostream>
#include <cstdio>
using namespace std;

int a[500][500], b[500][500], c[500][500];

int n;

int main()
{
	n = 10;
	for (int i = 1; i <= 10; i = i + 1) {
		if (i <= 5) {
			cout << n * n * i << endl;
		}
	}
	return 0;
}
