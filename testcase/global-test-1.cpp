#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 5000;
int a[MAXN];
const int n = MAXN;

int modify()
{
	int ans = 0;
	for (int i = 1; i < n; i = i + 1)
		a[i] = a[i] + 1;
	return ans;
}

int main()
{
	int ans = 0;
	for (int i = 0; i < n; i = i + 1) {
		ans = ans + a[1];
		ans = ans + modify();
	}
	cout << ans << endl;
	return 0;
}
