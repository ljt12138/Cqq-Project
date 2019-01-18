#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int a = 1, b = 3, c = 6, d = 7, e = 12, ans = 0;
	int n;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1) {
		ans = ans + a + b * (c + d * e);
	}
	cout << ans << endl;
	return 0;
}
