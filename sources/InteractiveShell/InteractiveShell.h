//
// InteractiveShell
// A replacement for CmdLineParser class
//
// Copyright (C) 2005-2016 byLAPLJ.
// All rights reserved.
//
// Uses readline GNU library.
// This class implements the singleton pattern.
//
// Typical use:
//
//	using namespace colibry;
//
//  // All these forms are equivalent
//	namespace Cmd {
//		void cmd1(const Tokens& tks, void* context);
//		DECLARE_ISHELL_FUNC(cmd2);
//		DECLARE_ISHELL_FUNC(exit);
//		DECLARE_ISHELL_FUNC(help);
//	};
//
//	InteractiveShell* ish = InteractiveShell::Instance();
//	ish->SetCtx(&mycontext);		// set context for commands (optional)
//
//	ish->RegisterCmds()
//		("cmd1", Cmd::cmd1, "this is command 1", {"group"})
//		("cmd2", Cmd::cmd2, "this is command 2", {"group", "group2"})
//		("exit", Cmd::exit, "exit the program")
//		("help", Cmd::help, "list commands");
//
//  Tokens args_cmd1 = { "-m", "--minor", "maxi" };
//  ish->SetArg0Options("cmd1", args_cmd1);
//
//	string cmd;
//	do {
//    	try {
//			cmd = ish->ReadExec("> ");
//    	} catch (const EmptyLine&) {
//    	} catch (const UnknownCmd& uc) {
//    		cerr << "\tUnknown command: " << uc.what() << endl;
//    	} catch (const SyntaxError& s) {
//    		cerr << "\tUsage: " << ish->Doc(s.what()) << endl;
//    	}
//	} while (cmd != "exit");
//
//	ISHELL_FUNC_FULL(Cmd:cmd1,args,context)
//	{
//		// body needs args and context
//	}
//
//	ISHELL_FUNC(Cmd::cmd2,args)
//	{
//		if (args.size()<2)
//			throw SyntaxError{args[0]};
//		// body, when tokens/args are needed => args
//	}
//
//	ISHELL_FUNC_NOARGS(Cmd::exit)
//	{
//		// body, no args are needed
//	}
//
// With:
//		ish->help({"group2"})
// cmd1 will not be shown.
//

#ifndef __InteractiveShell_H__
#define __InteractiveShell_H__

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>

// Hook Function Macros (helpers)
#define HOOK_FUNCTION(cmdf,tks,ctx) 	void cmdf(const colibry::Tokens& tks, void* ctx)
#define DECLARE_ISHELL_FUNC(cmdf)		HOOK_FUNCTION(cmdf,_tks,_ctx)
#define ISHELL_FUNC_FULL(cmdf,tks,ctx)	HOOK_FUNCTION(cmdf,tks,ctx)
#define ISHELL_FUNC(cmdf,tks)			HOOK_FUNCTION(cmdf,tks,_ctx)
#define ISHELL_FUNC_NOARGS(cmdf)		HOOK_FUNCTION(cmdf,_tks,_ctx)

namespace colibry {

	using Tokens = std::vector<std::string>;
	using Groups = std::vector<std::string>;
    using HookFunction = void (*)(const Tokens& tks, void* context);

	struct Command {
		Command() : func{nullptr} {}
		Command(const std::string& i,
			HookFunction f,
			const std::string& d,
			const Groups& gs) :
			id{i},func{f},doc{d},groups{gs} {}

		std::string id;		// command's name
		HookFunction func;	// function to be called when id is typed
		std::string doc;	// menu syntax help
		Groups groups;		// command groups
		Tokens arg0_opts;	// options for args (after command name)
	};

	//
	// Exceptions
	//

	class UnknownCmd : public std::runtime_error {
	public:
		UnknownCmd(const std::string& w) : std::runtime_error{w} {}
	};

	class SyntaxError : public std::runtime_error {
	public:
		SyntaxError(const std::string& w) : std::runtime_error{w} {}
	};

	class EmptyLine : public std::runtime_error {
	public:
		EmptyLine() : std::runtime_error{"EmptyLine"} {}
	};

	//
	// EasyInit: to help registering new commands
	//

	class InteractiveShell;

	class EasyInit {
	public:
		EasyInit(InteractiveShell* owner) : m_owner{owner} {}
		EasyInit& operator()(const Command& c);
		EasyInit& operator()(const std::string& id,
			HookFunction hook,
			const std::string& doc,
			const Groups& gs=Groups());
	private:
		InteractiveShell* m_owner;
	};

    //
    // InteractiveShell
    //

	class InteractiveShell {
	public:
		static InteractiveShell* Instance(); // obtain obj pointer (singleton)
		virtual ~InteractiveShell();

		virtual void SetCtx(void* ctx) { m_context = ctx; }
		virtual void SetArg0Options(const std::string& cmd, const Tokens& opts);
		virtual EasyInit RegisterCmds() { return EasyInit(this); }
		virtual void Exec(const std::string& cmd_line, void* ctx=nullptr);	// overrides global context
		virtual void Help(const Groups& gs=Groups()); // default = all groups
		virtual std::string Doc(const std::string& cmd);
		virtual std::string readline(const std::string& prompt); // may return an empty string
		virtual std::string ReadExec(const std::string& prompt);
		virtual void enable_file_completion(bool fcomp = true) { s_file_completion = fcomp; }

	protected:

		InteractiveShell();    // constructors are protected => singleton
		InteractiveShell(const InteractiveShell&);	// prevent copies
		InteractiveShell& operator=(const InteractiveShell&);

		std::vector<Command> m_cmdvec;
		Command& find_command(const std::string& cmd);	// may throw UnknownCommand
		void addcmd(const Command& c);

		static void Parse(const std::string &line, Tokens &pv);	// line => tokens
		// readline
		static char** my_completion(const char* text, int start, int end);
		static char* command_generator(const char* text, int state);
		static char* args_generator(const char* text, int state);

		friend class EasyInit;

	private:

		static InteractiveShell* s_pinstance;
		void* m_context;
		static bool s_file_completion;		// defaults to false
		static Tokens* s_args0;				// argument options of the current command
	};
};

#endif
