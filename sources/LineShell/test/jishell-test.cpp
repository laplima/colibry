#include <fmt/format.h>
#include <fmt/ostream.h>
#include <string>
#include <vector>
#include "../LineShell.h"

using fmt::print;
using namespace colibry;
using namespace colibry::lineshell;

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
// LineShell

int main()
{
	MyCommands cmds;		// observer
	LineShell sh{cmds};		// subject

	// PersistenceManager::load_file(sh,"../cmds.json");
	PersistenceManager::load_str(sh,R"([
		{
			"help": {
				"desc": "show help",
				"args": []
			}
		},
		{
			"exit": {
				"desc": "exit program",
				"args": []
			}
		},
		{
			"list": {
				"desc": "list projects or supervisors",
				"args": [
					{
						"projects": {
							"desc": "list projects",
							"args": []
						}
					},
					{
						"supervisors": {
							"desc": "list supervisors",
							"args": []
						}
					}
				]
			}
		}
	])");
	sh.add_cmd("list supervisors João Felipe", "Joca", 2);
	sh.add_cmd("list supervisors João Pedro", "Jope", 2);

	// sh.add_cmd(Command::create("list")
	// 	.described_as("list projects or supervisors")
	// 	.executed_by(cmds.make_list)
	// 	.with_argument("projects")
	// 	.with_argument("supervisors")
	// );

	sh.set_prompt("=> ");
	sh.cmdloop();
}
