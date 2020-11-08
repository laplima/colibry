//
//
//
//                OBSOLETE! Use OIShell instead.
//
//
//
// ISHELL - new Interactive Shell
//
// (C) 2020 by Luiz Lima Jr.
// All rights reserved.
//
// Reviewed in 2020 from the InteractiveShell class (now considered obsolete).
//
// On macOS:
//     This file must be compiled using the readline library located at
//     /opt/local/[include,lib]
//
// EXAMPLE:
//
// struct Context {		// optional context (data for commands)
//     int data;
// };
//
// void cmd1(const colibry::Arguments& args, Context* c)
// {
//     // Arguments = vector<string>
//     // implementation
// }
//
// int main()
// {
//     Context c = { 123 };
//
//     auto ish = colibry::IShell<Context>::instance();
//     // registration will keep the orden
//     ish->register_cmds()
//              ("command", cmd1, "description")
//              ("other", other, "description of other command");
//
//     ish->set_arg0_options("cmd1", {"opt1", "opt2", "opt3"});
//
//     while (true) {         // main loop
//         try {
//             string c = ish->read_exec("> ");
//         } catch (const colibry::UnknownCommand& uc) {
//             cerr << "unknown command: " << uc.what() << endl;
//         } catch (const colibry::ExitCommand&) {
//             break;
//         }
//     }
// }
//

#ifndef __ISHELL_H__
#define __ISHELL_H__

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <readline/readline.h>
#include <readline/history.h>

namespace colibry {

	// --------------------------------------------------------------------------

	class UnknownCommand : public std::runtime_error {
	public:
		UnknownCommand(const std::string& s) : std::runtime_error{s} {}
	};

	class SyntaxError : public std::runtime_error {
	public:
		SyntaxError(const std::string& w) : std::runtime_error{w} {}
	};

	class ExitCommand : public std::exception {
	public:
		ExitCommand() {}
	};

	using Arguments = std::vector<std::string>;

	// --------------------------------------------------------------------------

	static inline Arguments tokenize(const std::string& line);
	static inline void ltrim(std::string& s);
	static inline void rtrim(std::string& s);
	static inline void trim(std::string& s);

	// --------------------------------------------------------------------------

	namespace readlinens {		// readline-related
		static inline int quote_detector(char *line, int index);
		static inline std::string escape(const std::string& original);
		static char* dupstr(const std::string& s);
	}

	// --------------------------------------------------------------------------

	template<typename C=void>
	class IShell {
	public:
		using CmdFunction = std::function<void(const Arguments&, C*)>;
	public:

		// ---------- CmdData
		struct CmdData {
			CmdData() {}
			CmdData(CmdFunction func, const std::string& description);
			CmdFunction f;			// function to be called when id is typed
			std::string desc;		// menu syntax help
			Arguments arg0_opts;	// options for args (after command name)
		};

		// ---------- EasyInit
		class EasyInit {
		public:
			EasyInit(IShell<C>* owner) : owner_{owner} {}
			EasyInit& operator()(const std::string& cmd, const CmdData& c);
			EasyInit& operator()(const std::string& cmd, CmdFunction func, const std::string& description);
		private:
			IShell<C>* owner_;
		};

	protected:

		IShell();
		IShell(const IShell&) = delete;
		IShell& operator=(const IShell&) = delete;

		virtual CmdData& find_command(const std::string& cmd);
		virtual void exec(const Arguments& args, C* ctx);
		virtual void add_cmd(const std::string& cmd, const CmdData& c);

		// readline-related
		static bool file_completion;	// UNUSED
		static Arguments* args0;		// argument options of the current command

		static char* command_generator(const char* text, int state);
		static char* args_generator(const char* text, int state);
		static char** my_completion(const char* text, int start, int end);

	public:

		static IShell<C>* instance();

		virtual std::string readline(const std::string& prompt);
		virtual void set_arg0_options(const std::string& cmd, const Arguments& opts);
		virtual void exec(const std::string& cmdline, C* ctx=nullptr);
		virtual std::string read_exec(const std::string& prompt, C* ctx=nullptr);
		virtual void help(std::ostream& os=std::cout);
        virtual std::string docs(const std::string& cmd);
		virtual EasyInit register_cmds() { return EasyInit{this}; }

	protected:

		using CommandMap = std::map<std::string, CmdData>;
		CommandMap cmap_;
		std::vector<typename CommandMap::iterator> help_order_;
	};

	// ---------------------------------------------------------------------

	// CmdData ---------
	template<typename C>
	IShell<C>::CmdData::CmdData(CmdFunction func, const std::string& description)
		: f{func}, desc{description}
	{}

	// EasyInit --------
	template<typename C>
	typename IShell<C>::EasyInit& IShell<C>::EasyInit::operator()(const std::string& cmd,
		const CmdData& c)
	{
	    owner_->add_cmd(cmd, c);
	    return *this;
	}

	// EasyInit --------
	template<typename C>
	typename IShell<C>::EasyInit& IShell<C>::EasyInit::operator()(const std::string& cmd,
		CmdFunction func,
		const std::string& description)
	{
	    owner_->add_cmd(cmd, CmdData{func,description});
	    return *this;
	}

	// ---------------------------------------------------------------------

	template<typename C>
	bool IShell<C>::file_completion = false;

	template<typename C>
	Arguments* IShell<C>::args0 = nullptr;

	template<typename C>
	IShell<C>::IShell()
	{
		// rl_readline_name = "InteractiveShell";		// doesn't seem to be needed...
		rl_attempted_completion_function = my_completion;
		rl_completer_quote_characters = "'\"";
		static char space[2] = " ";
		rl_completer_word_break_characters = space;
		rl_char_is_quoted_p = &readlinens::quote_detector;

		// default commands: help() + exit()

		auto dflt_help = [this](const Arguments&, C*) {
			this->help();
		};

		auto dflt_exit = [this](const Arguments&, C*) {
			throw ExitCommand{};
		};

		register_cmds()
			("help",dflt_help,"list available commands")
			("exit",dflt_exit,"exit program");
	}

	template<typename C>
	IShell<C>* IShell<C>::instance()
	{
		static IShell<C> instance_;		// if already created, won't do anything
		return &instance_;
	}

	template<typename C>
	typename IShell<C>::CmdData& IShell<C>::find_command(const std::string& cmd)
	{
		if (cmap_.count(cmd) == 0)
			throw colibry::UnknownCommand(cmd);
		return cmap_[cmd];
	}

	template<typename C>
	void IShell<C>::exec(const Arguments& args, C* ctx)
	{
		auto c = find_command(args[0]);
		c.f(args,ctx);
	}

	template<typename C>
	void IShell<C>::add_cmd(const std::string& cmd, const CmdData& c)
	{
		cmap_[cmd] = c;
		help_order_.push_back(cmap_.find(cmd));
	}

	template<typename C>
	std::string IShell<C>::readline(const std::string& prompt)
	{
		std::string cmd;
		char* cstr = ::readline(prompt.c_str());
		if (cstr != nullptr) {
			// acquire ownership
			std::unique_ptr<char[]> line{cstr};
			cmd = line.get();
			colibry::trim(cmd);
			if (!cmd.empty())
				::add_history(cmd.c_str());
		}
		return cmd;
	}

	template<typename C>
	void IShell<C>::set_arg0_options(const std::string& cmd, const Arguments& opts)
	{
		CmdData& c = find_command(cmd);
		c.arg0_opts = opts;
	}

	template<typename C>
	void IShell<C>::exec(const std::string& cmdline, C* ctx)
	{
		if (cmdline.empty())
			return;
		auto args = colibry::tokenize(cmdline);
		exec(args, ctx);
	}

	template<typename C>
	std::string IShell<C>::read_exec(const std::string& prompt, C* ctx)
	{
		std::string s = readline(prompt);
		exec(s,ctx);
		return s;
	}

	template<typename C>
	void IShell<C>::help(std::ostream& os)
	{
		// std::vector<std::string> sorted;

		// Find max command size
		unsigned short max{0};
		for (auto& [cmd, cdata] : cmap_) {
			// sorted.emplace_back(cmd);
			if (std::size(cmd) > max)
				max = std::size(cmd);
		}
		max += 4;

		// std::sort(sorted.begin(), sorted.end());

		auto abstab = [](unsigned short t) -> std::string {
			std::string esc{char(0x1b)};
			return esc + "[80D" + esc + "[" + std::to_string(t) + "C";
		};

		for (auto &c : help_order_)
			os << std::setw(max) << c->first << abstab(max) << " : " << c->second.desc << std::endl;
	}

    template<typename C>
    std::string IShell<C>::docs(const std::string& cmd)
    {
        auto cdata = find_command(cmd);
        std::ostringstream ss;
        ss << cdata.desc;
        if (!cdata.arg0_opts.empty()) {
            ss << "\n" << "(";
            bool first = true;
            for (auto& o : cdata.arg0_opts)
                if (first) {
                    first = false;
                    ss << o;
                } else
                    ss << ", " << o;
            ss << ")";
        }
        return ss.str();
    }

	// readline-related ---------------------------------------------------

	template<typename C>
	char* IShell<C>::command_generator(const char* text, int state)
	{
		static IShell<C>* ish = instance();

		static int len;
		static auto index = std::begin(ish->cmap_);

		if (state == 0) {
			index = std::begin(ish->cmap_);
			len = std::size(std::string{text});
		}

		// std::cout << text << std::endl;

		auto ss = std::find_if(index, std::end(ish->cmap_),
			[text](const auto& e) {
				return (e.first.substr(0,len) == text);
		});

		index = ss;
		if (ss == std::end(ish->cmap_))		// not found
			return nullptr;

		// auto &[cmd,cdata] = ss;

		// duplicate cmd
		++index;
		return readlinens::dupstr(ss->first);
	}

	template<typename C>
	char* IShell<C>::args_generator(const char* text, int state)
	{
		static int len, list_index;	// must be static (for multiple calls)

		if (state == 0) {	// is this the 1st time this generator is called?
			len = std::string{text}.size();
			list_index = 0;
		}

		// search for the next match
		if (args0 == nullptr)
			return nullptr;

		while (list_index < args0->size()) {
			std::string arg{(*args0)[list_index]};
			++list_index;
			if (!rl_completer_quote_characters)
			// if (!rl_completion_quote_character)
				arg = readlinens::escape(arg);
			if (arg.substr(0,len) == text)
				return readlinens::dupstr(arg.c_str());
		}

		return nullptr;
	}

	template<typename C>
	char** IShell<C>::my_completion(const char* text, int start, int end)
	{
		if (!file_completion)
			rl_attempted_completion_over = 1;	// so that it won't fall back to file
												// (when there are no matches)
		char** matches = (char**)nullptr;
		if (start == 0)		// first command in line
			matches = rl_completion_matches(text,command_generator);
		else {
			// completion of other parameters
			// find command in buffer
			std::istringstream cline{rl_line_buffer};
			std::string cmd;
			cline >> cmd;

			auto ish = instance();	// current ish

			try {
				CmdData& cc = ish->find_command(cmd);
				if (cc.arg0_opts.size() == 0)
					return matches;
				args0 = &cc.arg0_opts;
				matches = rl_completion_matches(text, args_generator);
			} catch(colibry::UnknownCommand&) {
			}
		}
		return matches;
	}

	// ---------------------------------------------------------------------

	inline Arguments tokenize(const std::string& line)
	{
		Arguments v;
		std::istringstream is{line};
		std::string tk;
		while (is >> tk)
			v.push_back(tk);
		return v;
	}

	inline void ltrim(std::string& s)
	{
		s.erase(std::begin(s),
			std::find_if(std::begin(s), std::end(s), [](auto ch) {
			return !(std::isspace(ch) || ch=='\t');
		}));
	}

	inline void rtrim(std::string& s)
	{
		s.erase(std::find_if(std::rbegin(s), std::rend(s), [](auto ch) {
			return !(std::isspace(ch) || ch=='\t');
		}).base(), std::end(s));
	}

	inline void trim(std::string& s)
	{
		ltrim(s);
		rtrim(s);
	}

	// ---------------------------------------------------------------------

	static inline int readlinens::quote_detector(char *line, int index)
	{
		return (index > 0 && line[index - 1] == '\\' && !quote_detector(line, index - 1));
	}

	static inline std::string readlinens::escape(const std::string& original)
	{
		std::string escaped;
		for (char c : original) {
			if (c == ' ')
				escaped += "\\";
			escaped += c;
		}
		return escaped;
	}

	static char* readlinens::dupstr(const std::string& s)
	{
		char* d = new char[std::size(s)+1];
		std::copy(std::begin(s), std::end(s), d);
		d[std::size(s)] = '\0';
		return d;
	}

}	// end namespace

#endif
