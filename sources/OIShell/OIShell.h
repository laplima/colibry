//
// ALSO CONSIDER USING LINENOISE-based INTERACTIVE SHELL: LineShell
//
// OIShell: interactive shell using the Observer Design Pattern
// (C) 2020-21 by LAPLJ. All rights reserved.
//
// USAGE
// -----------------------------------------------------------------------------
//
// #include <colibry/OIShell.h>
//
// // Create command observer class
//
// class CommandHandler : public colibry::ISObserver {
// public:
//     CommandHandler()
//     {
//		    // help and exit are added by default
//			add_cmds()
//          	("test", { "one", "two"}, MWRAP(test), "test command");
//     }
// protected:
//     void test(const colibry::ishell::Arguments& pars)
//     // alternatively: CSIG(test,pars)
//     {
//           // args will NOT contain the command itself at [0]!
//           // implement command
//			 set_prompt(args[0]);
//     }
// };
//
// int main(int argc, char* argv[])
// {
//     CommandHandler ch;
//     colibry::OIShell::instance().run("> ");
// }
//
// -----------------------------------------------------------------------------
//
// If a specific cmd dispatch behavior is needed, dispatch can be overriden:
//
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

#ifndef OISHELL_H
#define OISHELL_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <stdexcept>
#include <functional>

namespace colibry {

	class ISObserver;
	class OIShell;

	namespace ishell {

		// IShell types

		using Arguments = std::vector<std::string>;
		using CmdMethod = std::function<void(const colibry::ishell::Arguments&)>;

		// is there another way of doing this without using macros??
		// Method (Command) Wrapper
		#define MWRAP(fn) [this](const colibry::ishell::Arguments& a) { this->fn(a); }
		// Command signature
		#define CSIG(cmd, args) void cmd(const colibry::ishell::Arguments& args)

		struct CmdData {
			CmdData() = default;
			CmdData(const Arguments& a, const CmdMethod& f, const std::string& desc);

			Arguments args;				// options for args (after command name)
			CmdMethod fn;
			std::string description;	// menu syntax help
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
				const ishell::CmdMethod& f,
				const std::string& description);
			EasyInit& operator()(const std::string& cmd,
				const ishell::CmdMethod& f,
				const std::string& description);	// no params
		private:
			ISObserver* owner_;
		};

	}

	// -------------------------------------------------------------------------

	// Client: to be derived from

	class ISObserver {
	public:
		virtual void dispatch(const std::string& cmd, const ishell::Arguments& args);
	public:
		ISObserver() noexcept;
		ishell::EasyInit add_cmds() { return ishell::EasyInit{this}; }
	protected:
		// default commands (may be overriden -- automatic dispatch)
		virtual void exit(const ishell::Arguments&);
		virtual void help(const ishell::Arguments&);
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
		[[nodiscard]] bool is_valid(const std::string& c) const {
			return cmap_.count(c) > 0; }
		void set_prompt(const std::string& np);
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
		void run(const std::string& prompt="");
		void stop() { loop_ = false; }
		void set_prompt(const std::string& np) { prompt_ = np; }
	private:
		static OIShell instance_;
		static char* command_generator(const char* text, int state);
		static char* args_generator(const char* text, int state);
		static char** my_completion(const char* text, int start, int end);
		static const ishell::Arguments* args0_;	// current cmd args
	public:
		OIShell(const OIShell&) = delete;
		OIShell& operator=(const OIShell&) = delete;
	private:
		OIShell();
		void notify(const ishell::Arguments& args);
		std::string readline(const std::string& prompt);
	private:
		std::mutex lock_;
		ISObserver* observer_ = nullptr;
		bool loop_;
		std::string prompt_;
	};

}

#endif
