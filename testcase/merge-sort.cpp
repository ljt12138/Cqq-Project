#include <iostream>
#include <cstdio>
using namespace std;

int a[1000001], tmp[1000001];
const int n = 1000000;

int merge_sort(int L, int R)
{
	if (L == R) return 0;
	int mid = (L+R)/2, Lp, Rp;
	merge_sort((Lp = L), mid);
	merge_sort((Rp = mid+1), R);
	int top = L;
	for (; Lp <= mid && Rp <= R; ) {
		if (a[Lp] <= a[Rp]) {
			tmp[top] = a[Lp];
			Lp = Lp + 1;
			top = top + 1;
		} else {
			tmp[top] = a[Rp];
			Rp = Rp + 1;
			top = top + 1;
		}
	}
	while (Lp <= mid) {
		tmp[top] = a[Lp];
		Lp = Lp + 1;
		top = top + 1;
	}
	while (Rp <= R) {
		tmp[top] = a[Rp];
		Rp = Rp + 1;
		top = top + 1;
	}
	int i;
	for (i = L; i <= R; i = i + 1)
		a[i] = tmp[i];
}

int main()
{
	int x = 31, y = 123, z = 1000000009;
	for (int i = 1; i <= n; i = i + 1)
		a[i] = (x * a[i-1] + y) % z;
	merge_sort(1, n);
	for (int i = 1; i <= n; i = i + 1)
		cout << a[i];
	cout << endl;
	return 0;
}
