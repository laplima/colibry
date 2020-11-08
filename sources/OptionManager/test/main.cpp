#include <iostream>
#include <string>
#include <functional>
#include "../OptionManager.h"

using std::cout;
using std::endl;
using std::string;
using colibry::OptionManager;

int main(int argc, char* argv[])
{
	using namespace std;

	OptionManager om{argc, argv};

	if (om.has("help"))
		cout << "HELP ME!" << endl;
}
