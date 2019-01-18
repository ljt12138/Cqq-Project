#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	const int n = 10000000;
	int ans = 0, x = 10, y = 20;
	for (int i = 0; i < n; i = i + 1)
		ans = ans + ((x * y + x ) * x + y) * i;
	cout << ans << endl;
	return 0;
}
