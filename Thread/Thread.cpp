#include <iostream>

using namespace std;

void Print()
{
	cout << "Finished!" << endl;
}

template<typename T, typename... Args>
void Print(T first, Args... args)
{
	cout << first << " / ";
	Print(args...);
}

int main()
{
	Print(1, 2, 3.14f, "Hello world", 'a');

	return 0;
}