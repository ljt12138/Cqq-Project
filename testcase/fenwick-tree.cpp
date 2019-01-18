#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 1000000;
int c[MAXN + 1], lowbit[MAXN + 1];
int n;

int modify(int pos, int dt)
{
	for (; pos <= n; pos = pos + lowbit[pos])
		c[pos] = c[pos] + dt;
	return 0;
}

int query(int pos)
{
	int ans = 0;
	for (; pos; pos = pos - lowbit[pos])
		ans = ans + c[pos];
	return ans;
}

int init()
{
	for (int i = 1; i <= n; i = i + 1) {
		if (i % 2 == 1) {
			lowbit[i] = 1;
		} else {
			lowbit[i] = lowbit[i/2] * 2; 
		} 
	}
}

int main()
{
	cin >> n;
	init();
	int ans = 0;
	for (int i = 1; i <= n; i = i + 1)
		modify(i, i);
	for (int i = 1; i <= n; i = i + 1)
		ans = ans ^ (i * query(i));
	cout << ans << endl;
	return 0;
}
