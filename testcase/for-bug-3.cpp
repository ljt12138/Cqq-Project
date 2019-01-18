#include <iostream>
#include <cstdio>
using namespace std;

int a = 0;

int test()
{
	int n = 1;
	for (int i = 2; (a = a + 1); i = i + 1) 
		return n;
	return 0;
}

int main()
{
	test();
	cout << a << endl;
	return 0;
}
