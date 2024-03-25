#include <iostream>

using namespace std;

void SetReference(int&& num)
{
	cout << num << endl;
	num = 12;
	cout << num << endl;
}

int main()
{
	SetReference(5);

	return 0;
}