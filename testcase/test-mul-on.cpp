#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int n, ans = 0;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1)
		ans = i + 3 * ans;
	cout << ans << endl;
	return 0;
}
