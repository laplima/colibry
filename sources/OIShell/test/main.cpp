#include <iostream>
#include <string>
#include <functional>
#include "../OIShell.h"

using std::cout;
using std::endl;
using std::string;
using colibry::ishell::Arguments;

class CommandHandler : public colibry::ISObserver {
public:

	void dispatch(const string& cmd, const Arguments& args) override
	{
		if (!is_valid(cmd)) {
			std::cerr << "Unknown command" << endl;
			return;
		}

		if (cmd == "exit")
			exit();
		else if (cmd == "help")
			help();
		else if (cmd == "test")
			test(args);
	}

protected:

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

	auto& ish = colibry::OIShell::instance();
	CommandHandler ch;
	ch.add_cmds()
		("help", "list commands")
		("exit", "exits the program")
		("test", {"one", "two"}, "test one or two");

	ish.run("> ");
}
