//
// Copyright (C) 2005-2006 by LAPLJ.
// All rights reserved.
//
// Uses readline GNU library.
//
// This class implements the singleton pattern.
//
// Typical use:
//
//   using namespace colibry;
//
//   namespace Cmd {
//       DECLARE_HOOK_FUNCTION(comd1);  // = int cmd1(const StringVec& sv, void* args);
//       DECLARE_HOOK_FUNCTION(comd2);
//       DHFUNC(help);					// same as DECLARE_HOOK_FUNCTION
//       ...
//   };
//
//   Command cmds[] = {
//       {"comand1", Cmd::comd1, "This is one.", 0, 0},  // sort order, group
//       {"comand2", Cmd::comd2, "This is two.", 0, 0},
//       {"help", Cmd::help, "Help with...", 0, 0},
//       END_OF_COMMANDS
//   };
//
//   CmdLineParser* clp = CmdLineParser::Instance();
//
//   clp->RegisterCmds(cmds);
//
//   OR
//
//   clp->RegisterCmd("comd1",Cmd::cmd1,"<arg1> <arg2>"); // default group 0
//   clp->RegisterCmd("comd2",Cmd::cmd2,"",1); // group 1
//   clp->RegisterCmd("help",Cmd::help);
//   ...
//   string cmd;
//   do {
//    	try {
//    		cmd = clp->readline("> ");
//    		clp->Execute(cmd);
//    	} catch (const EmptyLine&) {
//    		// nothing
//    	} catch (const UnknownCmdException& uce) {
//    		cerr << "\tUnknown command." << endl;
//    	} catch (const SyntaxError& s) {
//    		cerr << "\tUsage: " << clp->Help(s.m_cmd) << endl;
//    	}
//   } while (cmd != "exit");
//
// Commands belonging to group 0 are always available (i.e., shown in help).
//


#ifndef __CMDLINEPARSER_H__
#define __CMDLINEPARSER_H__

#include <string>
#include <map>
#include <vector>
#include <iostream>

#define HOOK_FUNCTION(f_name,sv,args) \
	int f_name(const colibry::StringVec& sv, void* args)

#define DECLARE_HOOK_FUNCTION(func_name) HOOK_FUNCTION(func_name,sv,args)
//int func_name(const colibry::StringVec& sv, void* args)

#define DHFUNC(func_name) DECLARE_HOOK_FUNCTION(func_name)
//int func_name(const colibry::StringVec& sv, void* args)

namespace colibry {

	typedef std::vector<std::string> StringVec;

    // Pointer to int hook(StringVec&,void *)
	typedef int (*HookFunction)(const StringVec& sv, void* context);

	struct Command {
		std::string name;
		HookFunction func;
		std::string doc;
		unsigned short help_order;	// order to appear in help
		unsigned short group;		// operation group
	};

	#define END_OF_COMMANDS { "", (colibry::HookFunction)NULL, "", 0, 0 }

    //
    // Exceptions
    //

	struct UnknownCmdException {
		UnknownCmdException(std::string cmd) : m_cmd(cmd) {}
		friend std::ostream& operator<< (std::ostream& os,
			const UnknownCmdException& e)
		{
			return (os << "UNKNOWN: " << e.m_cmd);
		}
		std::string m_cmd;
	};

	struct SyntaxError {
		SyntaxError(std::string cmd) : m_cmd(cmd) {}
		friend std::ostream& operator<<(std::ostream& os, const SyntaxError& e)
		{
			return (os << e.m_cmd);
		}
		std::string m_cmd;
	};

	class EmptyLine {};

    //
    // CmdLineParser
    //

	class CmdLineParser {
	public:
		static CmdLineParser* Instance();

		virtual ~CmdLineParser();
		virtual void RegisterCmds(Command cmds[], unsigned short size);
		virtual void RegisterCmds(Command cmds[]); // end array with END_OF_COMMANDS
		virtual void RegisterCmd(const Command& cmd);
		virtual void RegisterCmd(const std::string& name,
			HookFunction hook,
			const std::string& doc="",
			const unsigned short group=0);
		virtual int Execute(const std::string& cmd_line,
					void* context=NULL); // should limit to these exceptions...
		//    throw (UnknownCmdException, SyntaxError);
		virtual void Help(const unsigned short group=0); // 0: all commands
		virtual std::string Help(const std::string& cmd); // formerly, Syntax
		virtual std::string readline(const std::string& prompt); // may return empty string

		static void Parse(const std::string &line, StringVec &pv);
		static int  Parse(char* line, char* argvs[]);

	protected:

		CmdLineParser();    // constructors are protected => singleton
		CmdLineParser(const CmdLineParser&);
		CmdLineParser& operator=(const CmdLineParser&);

		static char** my_completion(const char* text, int start, int end);
		static char* command_generator(const char* text, int state);
		Command* find_command(const std::string& cmd);

		typedef std::map<std::string,Command> CmdMap;

		CmdMap m_cmdmap;
		bool IsRegistered(const std::string& cmd);
		unsigned short m_cmdno;

	private:

		static CmdLineParser* m_pinstance;
	};
};

#endif
