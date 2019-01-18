#include <iostream>
#include <cstdio>
using namespace std;

int fun(int n)
{
	int ans = 0;
	for (int i = 1; i <= n; i = i + 1)
		for (int j = 1; j <= n; j = j + 1) {
			ans = ans + 1;
			int k = 0;
			for (int k = 0; (i + j + k) % 15 != 0; k = k + 1) {
				ans = ans + k;
				if (k == 2)
					break;
			}
		}
	return ans;
}

int main()
{
	int n;
	cin >> n;
	cout << fun(n) << endl;
	return 0;
}
