#include <iostream>
#include <string>
#include "../OIShell.h"

using std::cout;
using std::endl;

class UnknownCommand : public std::runtime_error {
public:
	UnknownCommand(const std::string& s) : std::runtime_error{s} {}
};

class SyntaxError : public std::runtime_error {
public:
	SyntaxError(const std::string& w) : std::runtime_error{w} {}
};

class CommandHandler : public colibry::ISObserver {
public:

	void dispatch(const colibry::ishell::Arguments& args) override {
		const std::string& cmd = args[0];
		const colibry::ishell::Arguments& pars = {args.begin()+1, args.end()};
		if (cmd == "exit")
			exit();
		else if (cmd == "help")
			help();
		else if (cmd == "test")
			test(pars);
		else
			throw UnknownCommand{args[0]};
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
