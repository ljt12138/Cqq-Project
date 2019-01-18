#include <iostream>
#include <cstdio>
using namespace std;

const int MAXN = 200;
int a[MAXN][MAXN][MAXN];

int main()
{
	int n;
	cin >> n;
	int ans = 0;
	for (int i = 0; i < n; i = i + 1) {
		for (int j = 0; j < n; j = j + 1) {
			for (int k = 0; k < n; k = k + 1) {
				int t = 1;
				a[i][j][k] = 1;
				while (t <= n) {
					ans = a[i][j][k] + ans;
					ans = a[i][j][k] + ans;
					ans = a[i][j][k] + ans;
					ans = a[i][j][k] + ans;
					ans = a[i][j][k] + ans;
					ans = a[i][j][k] + ans;
					t = t + 1;
				}
				ans = ans + a[k][j][i];
			}
		}
	}

	cout << ans << endl;
	return 0;
}
