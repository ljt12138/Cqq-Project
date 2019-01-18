#include <iostream>
#include <cstdio>
using namespace std;

int gcd(int a, int b)
{
	if (b == 0)
		return a;
	return gcd(b, a%b);
}

const int a = 19260817, b = 21102031, c = 998244353;
int x = 0;

int rand()
{
	return x = ((a * x + b) % c + c) % c;
}

int main()
{
	int n;
	cin >> n;
	for (int i = 1; i <= n; i = i + 1) {
		int a = rand();
		cout << a;
	}
	cout << endl;
	return 0;
}
