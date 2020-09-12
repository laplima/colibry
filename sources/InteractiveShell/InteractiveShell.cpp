/*
 *  InteractiveShell.cpp
 *  InteractiveShell
 *
 *  Created by Luiz Lima Jr. on Fri Apr 02 2004.
 *  Copyright (c) 2016 LAPLIMA. All rights reserved.
 *
 */

#include "InteractiveShell.h"
#include <cstring>
#include <sstream>
#include <algorithm>
#include <readline/readline.h>
#include <readline/history.h>
#include <set>

using namespace colibry;
using namespace std;

// Fixed-length tabs
#define ABSTAB(t) "\e[80D\e[" << t << "C"

// helpers prototypes
char* dupstr(const char* t);
char* crop(char* s);
int quote_detector(char *line, int index);
string escape(const string& original);

//
// EasyInit
//

EasyInit& EasyInit::operator()(const Command& c)
{
    m_owner->addcmd(c);
    return *this;
}

EasyInit& EasyInit::operator()(const std::string& id, HookFunction hook,
    const std::string& doc, const Groups& gs)
{
    m_owner->addcmd(Command{id,hook,doc,gs});
    return *this;
}

//
// InteractiveShell
//

InteractiveShell* InteractiveShell::s_pinstance = nullptr;

InteractiveShell* InteractiveShell::Instance()
{
    if (s_pinstance == nullptr)
	   s_pinstance = new InteractiveShell();
    return s_pinstance;
}

bool InteractiveShell::s_file_completion = false;
Tokens* InteractiveShell::s_args0 = nullptr;

InteractiveShell::InteractiveShell() : m_context{nullptr}
{
	// rl_readline_name = "InteractiveShell";		// doesn't seem to be needed...
    rl_attempted_completion_function = my_completion;
    rl_completer_quote_characters = "'\"";
    static char space[2] = " ";
    rl_completer_word_break_characters = space;
	rl_char_is_quoted_p = &quote_detector;
}

InteractiveShell::~InteractiveShell()
{
	if (s_pinstance != nullptr)
		delete s_pinstance;
    s_pinstance = nullptr;
}

InteractiveShell& InteractiveShell::operator=(const InteractiveShell& cl)
{
	// protected
    return *this;
}

Command& InteractiveShell::find_command(const string& name)
{
	auto p = find_if(m_cmdvec.begin(), m_cmdvec.end(),
		[&name](Command const& c) { return (c.id==name); } );
	if (p == m_cmdvec.end())
		throw UnknownCmd{name};
	return *p;
}

void InteractiveShell::addcmd(const Command& c)
{
	try {
		find_command(c.id);
	} catch (const UnknownCmd&) {
		m_cmdvec.push_back(c);
	}
}

void InteractiveShell::SetArg0Options(const std::string& cmd, const Tokens& opts)
{
	Command& c = find_command(cmd);
	c.arg0_opts = opts;
}

void InteractiveShell::Exec(const std::string& cmd_line, void* context)
{
    if (cmd_line.empty())
	   throw EmptyLine();

    // parse input string
    Tokens tks;
    Parse(cmd_line, tks);

    Command& c = find_command(tks[0]);

    if (c.func == nullptr)
    	return;

    if (context == nullptr)
    	context = m_context;
    c.func(tks,context);
}

string InteractiveShell::Doc(const std::string& cmd)
{
	Command& c = find_command(cmd);
	return c.doc;
}

void InteractiveShell::Help(const Groups& gs)
{
    // Find max command size
	unsigned short max{0};
	for (const auto& c : m_cmdvec)
		if (c.id.length() > max)
			max = c.id.length();
	max += 4;

	if (gs.empty()) {
		// show all commands
		for (const auto& c : m_cmdvec)
			cout << "   " << c.id << ABSTAB(max) << ": " << c.doc << endl;
	} else {
		set<string> printed;
		for (auto& g : gs) {
			for (const auto& c : m_cmdvec) {
				if (c.groups.empty() || find(c.groups.begin(),c.groups.end(),g)!=c.groups.end()) {
					if (printed.count(c.id) == 0) {
						printed.insert(c.id);
						cout << "   " << c.id << ABSTAB(max) << ": " << c.doc << endl;
					}
				}
			}
		}
	}
}

void InteractiveShell::Parse(const string &line, Tokens &tks)
{
	tks.clear();
	if (line.empty()) return;

	istringstream is{line};
	string token;
	while (is >> token)
		tks.push_back(token);
}

string InteractiveShell::readline(const std::string& prompt)
{
	char* line = ::readline(prompt.c_str());
	if (line == nullptr)
		return string{""};

	char* cropped = crop(line);

	if (*cropped == '\0') {
		delete [] line;
		return string{""};
	}
	string cmd{cropped};
	add_history(cropped);
	delete [] line;
	return cmd;
}

std::string InteractiveShell::ReadExec(const std::string& prompt)
{
	string s = readline(prompt);
	Exec(s);
	return s;
}

// readline library-related

char** InteractiveShell::my_completion(const char* text, int start, int end)
{
	if (!s_file_completion)
		rl_attempted_completion_over = 1;	// so that it won't fall back to file
											// (when there are no matches)
	char** matches = (char**)nullptr;
	if (start == 0)		// first command in line
		matches = rl_completion_matches(text,command_generator);
	else {
		// completion of other parameters
		try {
			// find command in buffer
			string txt{rl_line_buffer};
			string cmd = txt.substr(0,txt.find(" "));
			Command& c = Instance()->find_command(cmd);
			if (c.arg0_opts.size() == 0) {
				return matches;
			}
			s_args0 = &c.arg0_opts;
			matches = rl_completion_matches(text,args_generator);
		} catch (const UnknownCmd&) {
		}
	}
	return matches;
}

char* InteractiveShell::command_generator(const char* text, int state)
{
	static int len;
	static int list_index;

	InteractiveShell* clp = Instance();

	if (state == 0) {
		list_index = 0;
		len = strlen(text);
	}

	while (list_index != clp->m_cmdvec.size()) {
		if (clp->m_cmdvec[list_index].id.substr(0,len) == text) {
			char* cmd = dupstr(clp->m_cmdvec[list_index].id.c_str());
			++list_index;
			return cmd;
		}
		++list_index;
	}

	// no matches
	return nullptr;
}

char* InteractiveShell::args_generator(const char* text, int state)
{
	static int len, list_index;	// must be static (for multiple calls)

	if (state == 0) {	// is this the 1st time this generator is called?
		len = string{text}.size();
		list_index = 0;
	}

	// search for the next match
	while (list_index < s_args0->size()) {
		string arg{(*s_args0)[list_index]};
		++list_index;
		if (!rl_completion_quote_character)
			arg = escape(arg);
		if (arg.substr(0,len) == text)
			return dupstr(arg.c_str());
	}

	return nullptr;
}

// helpers

char* dupstr(const char* t)
{
	char* d = new char[strlen(t)+1];
	strcpy(d,t);
	return d;
}

char* crop(char* s)
{
    char *pi, *pf;
    for (pi=s; *pi==' '; pi++) ;
    if (*pi == '\0')
		return pi;
    for (pf=s+strlen(s)-1; pf>s && *pf==' '; pf--) {}
   	*(pf+1) = '\0';
    return pi;
}

int quote_detector(char *line, int index)
{
	return (index > 0 &&
		line[index - 1] == '\\' &&
		!quote_detector(line, index - 1));
}

string escape(const string& original)
{
	string escaped;
	for (char c : original) {
		if (c == ' ')
			escaped += "\\";
		escaped += c;
	}
	return escaped;
}
