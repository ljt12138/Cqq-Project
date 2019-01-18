#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 10000000 + 5;
const int A = 2, B = 17, mod = 998244353;

int a[MAXN];
int nxt[MAXN];
int n;

int init()
{
	for (int i = 1; i <= n; i = i + 1)
		a[i] = (a[i-1] * A + B) % mod;
	for (int i = 1; i <= n; i = i + 1)
		a[i] = a[i] % 2;
}

int kmp_init()
{
	nxt[1] = 0;
	int p = 0;
	for (int i = 2; i <= n; i = i + 1) {
		for (; p && a[p+1] != a[i]; )
			p = nxt[p];
		if (a[p+1] == a[i])
			p = p + 1;
		cout << p;
	}
}

int main()
{
	cin >> n;
	init();
	kmp_init();
	return 0;
}
