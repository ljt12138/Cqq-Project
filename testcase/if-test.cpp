#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int n;
	cin >> n;
	if (n % 2 == 0) {
		int c = 1;
		cout << c + n << endl;
	} else {
		int d = 2;
		cout << d + n << endl;
	}
	return 0;
}
