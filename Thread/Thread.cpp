#include <iostream>

using namespace std;

void A()
{
	cout << "A" << endl;
}

int main()
{
	A();

	[]() {cout << "Lamda" << endl; }();

	return 0;
}