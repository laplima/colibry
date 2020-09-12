#include <iostream>
#include <string>
#include <colibry/Exception.h>

using namespace std;
//using namespace colibry;

int main(int argc, char* argv[])
{
	try {
		string s{"Luiz"};
		ThrowIf(false,"My name is NOT " << s);
		ThrowIf(true,"My name is " << s);
	} catch (const colibry::Exception& e) {
		cerr << e.what() << endl;
	}
}
