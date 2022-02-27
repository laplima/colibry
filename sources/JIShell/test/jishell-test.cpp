#include <fmt/format.h>
#include <fmt/ostream.h>
#include <string>
#include <vector>
#include "JIShell.h"

using fmt::print;
using namespace colibry;

class MyCommands : public CmdObserver {
public:
	MyCommands()
	{
		bind()
			("list", FWRAP(make_list));
	}

	void exit_(const Stringv& args) override {
		print("terminating\n");
		CmdObserver::exit_(args);
	}

	void make_list(const Stringv& args)
	{
		print("LIST\n>>{}\n", fmt::join(args.begin(), args.end(), ", "));
	}
};

// template<typename C, std::string file_name>
// JIShell

int main()
{
	MyCommands cmds;		// observer
	JIShell sh{cmds};		// subject

	PersistenceManager::load("../cmds.json", sh);

	sh.set_prompt("=> ");
	sh.cmdloop();
}
