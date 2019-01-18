#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int n = 10;
	if (1) cout << 1;
	else if (2) cout << 2;
	else cout << 3;

	if (0) cout << 1;
	else if (1) cout << 2;
	else cout << 3;

	if (0) cout << 1;
	else if (0) cout << 2;
	else cout << 3;
	cout << endl;
	return 0;
}
