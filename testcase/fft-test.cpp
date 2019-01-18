#include <iostream>
#include <cstdio>
using namespace std;

const int mod = 12289, lgn = 16, g = 11;

int power(int a, int n, int mod)
{
	int ans = 1;
	for (; n; n = n / 2) {
		if (n % 2 == 1)
			ans = ans * a % mod;
		a = a * a % mod;
	}
	return ans;
}

int a[12289];

int NTT(int n, int lgn, int flag)
{
	int rev[12289];
	rev[0] = 0;
	for (int i = 1; i < n; i = i + 1)
		rev[i] = rev[ i / 2 ] / 2 + (i % 2) * (n / 2);
	for (int i = 1; i < n; i = i + 1)
		if (rev[i] < i) {
			int t = a[i];
			a[i] = a[rev[i]];
			a[rev[i]] = t;
		}	
        for (int k = 2; k <= n; k = k + k) {
		int dw = power(g, (mod - 1) / k, mod);
		if (flag == -1)
			dw = power(dw, mod-2, mod);
		for (int i = 0; i < n; i = i + k) {
			int w = 1;
			for (int j = 0; j < k / 2; j = j + 1) {
				int u, v;
				u = a[i + j] % mod;
				v = w * a[i + (k / 2) + j] % mod;
				a[i + j] = (u + v) % mod;
				a[i + (k / 2) + j] = ((u - v) % mod + mod) % mod;
				w = w * dw % mod;
			}
		}
	}
	if (flag == -1) {
		int invn = power(n, mod-2, mod);
		for (int i = 0; i < n; i = i + 1)
			a[i] = invn * a[i] % mod;
	}
}

int main()
{
	const int n = 1024, lgn = 10;
	int x = 123, y = 456, pre = 0;
	for (int i = 0; i < n; i = i + 1)
		pre = a[i] = (x * pre + y) % mod;
	NTT(n, lgn, 1);
	for (int i = 0; i < n; i = i + 1)
		a[i] = a[i] * a[i] % mod;
	NTT(n, lgn, -1);
	for (int i = 0; i < n; i = i + 1)
		cout << a[i];
	cout << endl;
	return 0;
}
