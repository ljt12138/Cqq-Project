#include <iostream>
#include <cstdio>
using namespace std;

const int n = 10;
const int m = n + 10, t = n + m;
int a[10];

int l;

int main()
{
	const int k = n + m + 1;
	int i;
        for (int i = 0; i < 10; i = i + 1) {
		cin >> a[i];
		cout << i << endl;
	}
	for (int i = 0; i < 10; i = i + 1)
		cout << a[i] << endl;
	return 0;
}
