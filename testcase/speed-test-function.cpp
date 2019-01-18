#include <iostream>
#include <cstdio>
using namespace std;

const int N = 524288;

int zkw[N + N];

int lc(int nd)
{ return nd + nd; }

int rc(int nd)
{ return nd + nd + 1; }

int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

int update(int nd)
{
	zkw[nd] = max(zkw[lc(nd)], zkw[rc(nd)]);
}

int query(int L, int R)
{
	int ans = 0;
	int i, j;
	i = L + (N - 1); j = R + (N - 1);
	for (; i <= j; ) {
		if (i % 2 == 1) {
			ans = max(ans, zkw[i]);
			i = i + 1;
		}
		if (j % 2 == 0) {
			ans = max(ans, zkw[j]);
			j = j - 1;
		}
		i = i / 2;
		j = j / 2;
	}
	return ans;
}

int modify(int pos, int dt)
{
	pos = pos + (N - 1);
	zkw[pos] = dt;
	for (pos = pos / 2; pos; pos = pos / 2)
		update(pos);
}

int main()
{
	int n;
	cin >> n;
	int i;
	for (i = 1; i <= n; i = i + 1) {
		int opt, L, R;
		cin >> opt >> L >> R;
		if (opt == 1) {
			modify(L, R);
		} else {
			if (L > R) {
				int t = L;
				L = R;
				R = t;
			}
			cout << query(L, R) << endl;
		}
	}
	return 0;
}
