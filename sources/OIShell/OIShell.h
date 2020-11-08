//
// OIShell: interactive shell using the Observer Design Pattern
// (C) 2020- by LAPLJ. All rights reserved.
//
// USAGE
// -----------------------------------------------------------------------------
//
// #include <colibry/OIShell.h>
//
// class CommandHandler : public colibry::ISObserver {
// public:
//     void dispatch(const std::string& cmd, const colibry::ishell::Arguments& args) override
//     {
//         if (is_valid(cmd)) {
//             if (cmd == "exit")
// 	               exit();       // default exit
//             else if (cmd == "help")
// 	               help();       // default help
//             else if (cmd == "test")
// 	               test(pars);
//             else
// 	               std::cerr << "invalid command" << std::endl;
//         }
//    }
// protected:
//     void test(const colibry::ishell::Arguments& pars)
//     {
//           // implement command
//     }
// };
//
// int main(int argc, char* argv[])
// {
//     auto& ish = colibry::OIShell::instance();
//     CommandHandler ch;
//     ch.add_cmds()
//         ("test", { "one", "two"}, "test command")
//         ("help", "list commands")      // no parms
// 	       ("exit", "exits the program"); // idem
//
//     ish.run("> ");
// }
//

#ifndef __OISHELL_H__
#define __OISHELL_H__

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <stdexcept>

namespace colibry {

	class ISObserver;
	class OIShell;

	namespace ishell {

		// IShell types

		using Arguments = std::vector<std::string>;

		struct CmdData {
			CmdData() {}
			CmdData(const Arguments& pars, const std::string& desc);

			std::string description;	// menu syntax help
			Arguments args;				// options for args (after command name)
		};

		using CmdMap = std::map<std::string, CmdData>;

		// ---------- Exceptions (for user convenience)

		class UnknownCommand : public std::runtime_error {
		public:
			UnknownCommand(const std::string& s) : std::runtime_error{s} {}
		};

		class SyntaxError : public std::runtime_error {
		public:
			SyntaxError(const std::string& w) : std::runtime_error{w} {}
		};

		// ---------- EasyInit

		class EasyInit {
		public:
			EasyInit(ISObserver* owner) : owner_{owner} {}
			EasyInit& operator()(const std::string& cmd, const CmdData& c);
			EasyInit& operator()(const std::string& cmd,
				const ishell::Arguments& args,
				const std::string& description);
			EasyInit& operator()(const std::string& cmd,
				const std::string& description);	// no params
		private:
			ISObserver* owner_;
		};

	}

	// -------------------------------------------------------------------------

	// Client: to be derived from

	class ISObserver {
	public:

		virtual void dispatch(const std::string& cmd, const ishell::Arguments& args) = 0;

	public:
		ISObserver() noexcept;
		ishell::EasyInit add_cmds() { return ishell::EasyInit{this}; }
	protected:
		// default commands (may be overriden -- no automatic dispatch, though)
		virtual void exit();
		virtual void help();
	protected:
		friend class ishell::EasyInit;
		friend class OIShell;
		void add_cmd(const std::string& cmd, const ishell::CmdData& cd)
		{
			if (cmap_.count(cmd) == 0) {
				regorder_.push_back(cmd);
				cmap_[cmd] = cd;
			}
		}
		const ishell::CmdMap& cmdmap() { return cmap_; }
		const ishell::Arguments* cmdargs(const std::string& cmd) {
			return &(cmap_.at(cmd).args); }
		bool is_valid(const std::string& c) const { return cmap_.count(c) > 0; }
	protected:
		ishell::CmdMap cmap_;
		ishell::Arguments regorder_;	// registration order of the commands
	};

	// -------------------------------------------------------------------------

	class OIShell {
	public:
		static OIShell& instance() { return instance_; }
		void observer(ISObserver *obs);
		ISObserver* observer() { return observer_; }
		void run(const std::string& prompt);
		void stop() { loop_ = false; }
	private:
		static OIShell instance_;
		static char* command_generator(const char* text, int state);
		static char* args_generator(const char* text, int state);
		static char** my_completion(const char* text, int start, int end);
		static const ishell::Arguments* args0_;	// current cmd args
	private:
		OIShell();
		~OIShell();
		OIShell(const OIShell&) = delete;
		OIShell& operator=(const OIShell&) = delete;
		void notify(const ishell::Arguments& args);
		std::string readline(const std::string& prompt);
	private:
		std::mutex lock_;
		ISObserver* observer_;
		bool loop_;
	};

}

#endif
