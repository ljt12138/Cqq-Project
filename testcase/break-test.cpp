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
			while (k <= 20) {
				ans = ans + k;
				k = k + 1;
				if ((i + j + k) % 15 == 0)
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
