#include <iostream>
#include <cstdio>
using namespace std;

int main()
{
	int i = 0;
	for (i = 1; 0; i = i + 1);
	while (0);
	cout << i << endl;
	return 0;
}
