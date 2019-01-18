#include <iostream>
#include <cstdio>
using namespace std;

int ans = 0;

int add()
{
	return ans = ans + 1;
}

int main()
{
	if (1 && add())
		cout << ans;
	if (0 && add())
		cout << ans;
	if (1 || add())
		cout << ans;
	if (0 || add())
		cout << ans;
	return 0;
}
