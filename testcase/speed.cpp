#include <iostream>
#include <cstdio>
using namespace std;

const int n = 100000000;
const int m = n + 10, t = n + m;
int a[n];

int l;

int main()
{
	for (int i = 1; i <= 100000000; i = i + 1) {
		a[i - 1] = i;
	}
	for (int i = 0; i < 10; i = i + 1) 
		cout << a[i] << endl;
	return 0;
}
