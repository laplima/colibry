#include <iostream>
#include <string>
// #include <readline/rltypedefs.h>
// #include <redline/readline.h>
#include <functional>
#include "../OIShell.h"

using std::cout;
using std::endl;
using std::string;
using colibry::ishell::Arguments;

class CommandHandler : public colibry::ISObserver {
public:
	CommandHandler()
	{
		add_cmds()
			("test", {"one", "two"}, SF(test), "test one or two");
	}
public:
	void test(const colibry::ishell::Arguments& pars) {
		cout << "test" << endl;
		bool first = true;
		cout << "[";
		for (auto& s : pars) {
			if (first) {
				cout << s;
				first = false;
			} else
				cout << "],[" << s;
		}
		cout << "]" << endl;
	}
};

int main(int argc, char* argv[])
{
	using namespace std;
	// using namespace std::placeholders; // _1, _2, ...
	// auto f = std::bind(cmd1,_1,_2);

	CommandHandler ch;
	colibry::OIShell::instance().run("> ");
}
