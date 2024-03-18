#include <iostream>
#include <Windows.h>

using namespace std;

int A(int a, int b)
{
	return 0;
}

using P = int(*)(int, int);

void Call(P p)
{
	p();
}

int main()
{
	P p = NULL;

	p = A;

	Call(p);

	return 0;
}