#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using namespace this_thread;

void Print()
{
	while (true)
	{
		printf("Hello World\n");
		sleep_for(1s);
	}
}

int main()
{
	thread t(Print);
	t.join();

	return 0;
}