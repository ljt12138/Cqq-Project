#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int n, ans = 0;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1) {
		ans = ans + (i + (i + (i + (i + (i + i)))));
		ans = ans * (i * (i * (i * (i * (i * i)))));
		ans = ans - i - i - i - i - i - i;
	}
	cout << ans << endl;
	return 0;
}
