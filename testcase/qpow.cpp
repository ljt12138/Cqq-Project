#include <iostream>
#include <cstdio>
using namespace std;

const int N = 65537, g = 3;

int power(int a, int n, int mod)
{
	int ans = 1;
	for (; n != 0; n = n / 2) {
		if (n % 2 == 1)
			ans = ans * a % mod;
		a = a * a % mod;
	}
	return ans;
}

int main()
{
	int n;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1) {
		int a, b, c;
		cin >> a >> b >> c;
		cout << power(a, b, c) << endl;
	}
	return 0;
}
