#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int n, ans = 0;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1)
		ans = ans + i * 2 * 3 * 4 * 5 * 6 * 7;
	cout << ans << endl;
	return 0;
}
