#include <iostream>
#include <functional>

using namespace std;

class Obj
{
public:
	Obj() {cout << "����" << endl; }
	~Obj() { cout << "�Ҹ�" << endl; }
public:
	void Hello() { cout << "hello world" << endl; }
};

int main()
{
	std::shared_ptr<Obj> ptr1 = std::make_shared<Obj>();
	cout << "Reference Count : " << ptr1.use_count() << endl;

	{
		//�Ѵ� ����
		//std::shared_ptr<Obj> ptr2 = std::move(ptr1);
		std::shared_ptr<Obj> ptr2 = ptr1;
		cout << "Reference Count : " << ptr1.use_count() << endl;

	}//ptr2 �Ҹ�

	cout << "Reference Count : " << ptr1.use_count() << endl;
	return 0;
}