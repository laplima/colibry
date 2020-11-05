//
// ObsIShell: IShell using the Observer Design Pattern
//


#include <sstream>
// #include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#include "OIShell.h"

// ----- helpers ---------------------------------------------------------------

namespace colibry {
	namespace ishell {

		Arguments split(const std::string& input)
		{
		    std::regex re{"\\s+"};
		    std::sregex_token_iterator first{std::begin(input), std::end(input), re, -1};
		    std::sregex_token_iterator last;
		    return {first, last};
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

		inline int quote_detector(char *line, int index)
		{
			return (index > 0 && line[index - 1] == '\\' && !quote_detector(line, index - 1));
		}

		inline std::string escape(const std::string& original)
		{
			std::string escaped;
			for (char c : original) {
				if (c == ' ')
					escaped += "\\";
				escaped += c;
			}
			return escaped;
		}

		char* dupstr(const std::string& s)
		{
			char* d = new char[std::size(s)+1];
			std::copy(std::begin(s), std::end(s), d);
			d[std::size(s)] = '\0';
			return d;
		}

		CmdData::CmdData(const Arguments& pars, const std::string& desc)
			: description_{desc}, args_{pars}
		{
		}

		EasyInit& EasyInit::operator()(const std::string& cmd, const CmdData& c)
		{
			owner_->add_cmd(cmd,c);
			return *this;
		}

		EasyInit& EasyInit::operator()(const std::string& cmd,
				const ishell::Arguments& args,
				const std::string& desc)
		{
			owner_->add_cmd(cmd, CmdData{args,desc});
			return *this;
		}

		EasyInit& EasyInit::operator()(const std::string& cmd,
				const std::string& desc)
		{
			owner_->add_cmd(cmd, CmdData{{},desc});
			return *this;
		}


	}
}

using namespace colibry;

// ----- ISObserver ------------------------------------------------------------

ISObserver::ISObserver() noexcept
{
	OIShell::instance().observer(this);
}

void ISObserver::exit()
{
	// default exit
	OIShell::instance().stop();
}

void ISObserver::help()
{
	// Find max command size
	unsigned short max{0};
	for (const auto& c : regorder_) {
		// sorted.emplace_back(cmd);
		if (std::size(c) > max)
			max = std::size(c);
	}
	max += 4;

	// std::sort(sorted.begin(), sorted.end());

	auto abstab = [](unsigned short t) -> std::string {
		std::string esc{char(0x1b)};
		return esc + "[80D" + esc + "[" + std::to_string(t) + "C";
	};

	for (const auto &c : regorder_)
		std::cout << std::setw(max) << c << abstab(max) << " : "
				  << cmap_[c].description_ << std::endl;

}

// ----- ISObserver ------------------------------------------------------------

OIShell OIShell::instance_;
const ishell::Arguments* OIShell::args0_ = nullptr;


OIShell::OIShell() : loop_{true}
{
	// rl_readline_name = "InteractiveShell";		// doesn't seem to be needed...
	rl_attempted_completion_function = my_completion;
	rl_completer_quote_characters = "'\"";
	static char space[2] = " ";
	rl_completer_word_break_characters = space;
	rl_char_is_quoted_p = &ishell::quote_detector;
}

OIShell::~OIShell()
{
}

void OIShell::observer(ISObserver *obs)
{
	observer_ = obs;
}

void OIShell::notify(const ishell::Arguments& args)
{
	if (observer_ != nullptr)
		observer_->dispatch(args);
}

void OIShell::run(const std::string& prompt)
{
	loop_ = true;
	while (loop_) {
		std::string cl = readline(prompt);
		ishell::trim(cl);
		if (cl.empty())
			continue;
		auto args = ishell::split(cl);
		notify(args);
	}
}

std::string OIShell::readline(const std::string& prompt)
{
	std::string cmd;
	char* cstr = ::readline(prompt.c_str());
	if (cstr != nullptr) {
		// acquire ownership
		std::unique_ptr<char[]> line{cstr};
		cmd = line.get();
		ishell::trim(cmd);
		if (!cmd.empty())
			::add_history(cmd.c_str());
	}
	return cmd;
}

// readline-related ---------------------------------------------------

char* OIShell::command_generator(const char* text, int state)
{
	static auto& ish = instance();
	if (ish.observer() == nullptr)
		return nullptr;

	auto& cmap = ish.observer()->cmdmap();

	static int len;
	static auto index = std::begin(cmap);

	if (state == 0) {
		index = std::begin(cmap);
		len = std::size(std::string{text});
	}

	auto ss = std::find_if(index, std::end(cmap),
		[text](const auto& e) {
			return (e.first.substr(0,len) == text);
	});

	index = ss;
	if (ss == std::end(cmap))		// not found
		return nullptr;

	// duplicate cmd
	++index;
	return ishell::dupstr(ss->first);
}

char* OIShell::args_generator(const char* text, int state)
{
	static int len, list_index;	// must be static (for multiple calls)

	if (state == 0) {	// is this the 1st time this generator is called?
		len = std::string{text}.size();
		list_index = 0;
	}

	// search for the next match
	if (args0_->empty())
		return nullptr;

	while (list_index < args0_->size()) {
		std::string arg{(*args0_)[list_index]};
		++list_index;
		if (!rl_completer_quote_characters) // if (!rl_completion_quote_character)
			arg = ishell::escape(arg);
		if (arg.substr(0,len) == text)
			return ishell::dupstr(arg.c_str());
	}

	return nullptr;
}

char** OIShell::my_completion(const char* text, int start, int end)
{
	// if (!file_completion)
	// 	rl_attempted_completion_over = 1;	// so that it won't fall back to file
	// 										// (when there are no matches)
	char** matches = (char**)nullptr;
	if (start == 0)		// first command in line
		matches = rl_completion_matches(text,command_generator);
	else {
		// completion of other parameters
		// find command in buffer
		std::istringstream cline{rl_line_buffer};
		std::string cmd;
		cline >> cmd;

		static auto& ish = instance();	// current ish
		if (ish.observer() == nullptr)
			return matches;

		try {
			args0_ = ish.observer_->cmdargs(cmd);
			if (args0_->size() == 0)
				return matches;
			matches = rl_completion_matches(text, args_generator);
		} catch(...) {
		}
	}
	return matches;
}

