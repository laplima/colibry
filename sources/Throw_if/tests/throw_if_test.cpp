#include <iostream>
#include <string>
#include <colibry/throw_if.h>

using namespace std;
using namespace colibry;

class MyException : public runtime_error {
public:
	MyException(const string& w) : runtime_error{w} {}
};

int main(int argc, char* argv[])
{
	try {
		int x = 1;

		throwif<MyException>(x>0,"HI");	// doesn't work
	} catch (const MyException& e) {
		cout << e.what() << endl;
	}
}