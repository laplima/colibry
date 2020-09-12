#include <iostream>
#include "../CmdLineParser.h"

using namespace std;
using namespace colibry;

namespace Cmd {
	DECLARE_HOOK_FUNCTION(comando1);
	DECLARE_HOOK_FUNCTION(comando2);
	DECLARE_HOOK_FUNCTION(exit);
	DHFUNC(help);
};

colibry::Command cmds[] = {
	{ "comando1", Cmd::comando1, "Um", 1, 1 },
	{ "comando2", Cmd::comando2, "Dois", 2, 2},
	{ "exit", Cmd::exit,"Sair", 3, 0},
	{ "help", Cmd::help, "Ajuda",4, 0},
	END_OF_COMMANDS
};

int main(int argc, char* argv[])
{
	CmdLineParser* clp = CmdLineParser::Instance();

	clp->RegisterCmds(cmds);

	string cmd;
	do {
		try {
			cmd = clp->readline("> ");
			clp->Execute(cmd);
		} catch (const EmptyLine&) {
			// nothing
		} catch (const UnknownCmdException& uce) {
			cerr << "\tUnknown command." << endl;
		} catch (const SyntaxError& s) {
			cerr << clp->Help(s.m_cmd) << endl;
		}
	} while (cmd != "exit");

    return 0;
}

DECLARE_HOOK_FUNCTION(Cmd::comando1)
{
	cout << "UM!!" << endl;
	return 0;
}

DECLARE_HOOK_FUNCTION(Cmd::comando2)
{
	cout << "DOIS!!" << endl;
	return 0;
}

DECLARE_HOOK_FUNCTION(Cmd::exit)
{
	cout << "exiting" << endl;
	return 0;
}

DECLARE_HOOK_FUNCTION(Cmd::help)
{
	CmdLineParser::Instance()->Help(1);
	return 0;
}
