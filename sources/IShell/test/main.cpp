#include <iostream>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "../IShell.h"

using std::cout;
using std::endl;

struct Context {
	int x;
	Context(int p) : x{p} {}
};

std::ostream& operator<<(std::ostream& os, const Context& c)
{
	return (os << c.x);
}

void cmd1(const std::vector<std::string>& args, Context* c)
{
	bool first = true;
	for (auto& s : args) {
		if (first) {
			cout << s;
			if (c != nullptr)
				cout << " (" << *c << ")";
			cout << ":" << endl;
			first = false;
		} else {
			cout << "\t" << s << endl;
		}
	}
}

void cmd2 (const colibry::Arguments& args, Context *c)
{
	bool first = true;
	for (auto& s : args) {
		if (first) {
			cout << s;
			if (c != nullptr)
				cout << " (" << *c << ")";
			cout << ":" << endl;
			first = false;
		} else {
			cout << "\t" << s << endl;
		}
	}
}

int main(int argc, char* argv[])
{
	using namespace std;
	// using namespace std::placeholders; // _1, _2, ...
	// auto f = std::bind(cmd1,_1,_2);

	Context c{123};

	auto ish = colibry::IShell<Context>::instance();
	ish->register_cmds()
		("dois", cmd2, "second")
		("um", cmd1, "this is the first command");

	ish->set_arg0_options("dois", {"par", "impar", "primo"});

	try {
		ish->exec("um ola",&c);
		ish->exec("dois tico no fuba");
		ish->exec("batata frita");
	} catch(const colibry::UnknownCommand& uc) {
		cerr << "Unknown: " << uc.what() << endl;
	}

	ish->help();

	string rc = ish->readline("> ");
	while (rc != "exit") {
		try {
			ish->exec(rc);
			rc = ish->readline("> ");
		} catch(const colibry::UnknownCommand& uc) {
			cerr << "Unknown: " << uc.what() << endl;
		} catch(const colibry::ExitCommand&) {
			cout << "Exiting" << endl;
			break;
		}
	}

    return 0;
}
