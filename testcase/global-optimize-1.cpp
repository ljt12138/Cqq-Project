#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 5000;
int a[MAXN];
const int n = MAXN;

int modify(int pos)
{
	return (a[pos] + 1) * 2;
}

int main()
{
	int ans = 0;
	for (int i = 0; i < n; i = i + 1) {
		for (int j = 0; j < n; j = j + 1) {
			ans = ans + a[i];
			ans = ans + modify(i);
		}
	}
	cout << ans << endl;
	return 0;
}
