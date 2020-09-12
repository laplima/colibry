/*
 *  CmdLineParser.cpp
 *  CmdLineParser
 *
 *  Created by Luiz Lima Jr. on Fri Apr 02 2004.
 *  Copyright (c) 2004-12 LAPLIMA. All rights reserved.
 *
 */

#include "CmdLineParser.h"
#include <cstring>
#include <sstream>
#include <algorithm>
#include <readline/readline.h>
#include <readline/history.h>

using namespace colibry;
using namespace std;

// Fixed-length tabs
#define ABSTAB(t) "\e[80D\e[" << t << "C"

// helpers prototypes
char* dupstr(const char* t);
char* crop(char* s);

class HelpEntry {
public:
    unsigned short order;
    string cmdhelp;
    HelpEntry(unsigned short o, const string& ch) : order(o), cmdhelp(ch) {}
    bool operator<(const HelpEntry& he) const { return (order<he.order); }
};

CmdLineParser* CmdLineParser::m_pinstance = NULL;

CmdLineParser* CmdLineParser::Instance()
{
    if (m_pinstance == NULL)
	m_pinstance = new CmdLineParser();
    return m_pinstance;
}

CmdLineParser::CmdLineParser() : m_cmdno(0)
{
    //rl_readline_name = "CmdLineParser";		// don't know what this is for...
    rl_attempted_completion_function = my_completion;
}

CmdLineParser::~CmdLineParser()
{
    m_pinstance = NULL;
}

CmdLineParser& CmdLineParser::operator=(const CmdLineParser& cl)
{
    return *this;
}

void CmdLineParser::RegisterCmds(Command cmds[], unsigned short size)
{
    for (unsigned short i=0; i<size; i++)
	RegisterCmd(cmds[i]);
}

void CmdLineParser::RegisterCmds(Command cmds[])
{
    int i = 0;
    while (!cmds[i].name.empty())
	RegisterCmd(cmds[i++]);
}

void CmdLineParser::RegisterCmd(const Command& cmd)
{
    m_cmdmap[cmd.name] = cmd;
}

void CmdLineParser::RegisterCmd(const std::string& name,
				HookFunction hook,
				const std::string& doc,
				const unsigned short group)
{
    m_cmdmap[name].name = name;
    m_cmdmap[name].func = hook;
    m_cmdmap[name].doc = doc;
    m_cmdmap[name].help_order = m_cmdno++;
    m_cmdmap[name].group = group;
}

int CmdLineParser::Execute(const std::string& cmd_line, void* context)
//throw (UnknownCmdException,SyntaxError)
{
    if (cmd_line.empty())
	throw EmptyLine();
    
    // parse input string
    StringVec sv;
    Parse(cmd_line, sv);

    if (IsRegistered(sv[0]))
	return m_cmdmap[sv[0]].func(sv,context);

    throw UnknownCmdException(sv[0]);
}

void CmdLineParser::Help(const unsigned short group)
{
    // Show commands of the group (0 == all)

    // Find max command size
    CmdMap::const_iterator it;
    unsigned short max=0;
    for (it=m_cmdmap.begin(); it!=m_cmdmap.end(); it++)
	if (it->first.length() > max)
	    max = it->first.length();

    // Create help map (ordered by registration order)

    vector<HelpEntry> help;
    
    string cmdsyn;
    for (it=m_cmdmap.begin(); it!=m_cmdmap.end(); it++) {
	if (group == 0 || it->second.group == group || it->second.group == 0) {
	    cmdsyn = it->first;
	    for (unsigned short i=0; i<max-it->first.length()+1; i++)
		cmdsyn += " ";
	    cmdsyn += ": ";
	    cmdsyn += it->second.doc;
	    help.push_back(HelpEntry(it->second.help_order,cmdsyn));
	    //helpmap[it->second.help_order] = cmdsyn;
	}
    }

    sort(help.begin(),help.end());

    for (unsigned int i=0; i<help.size(); i++)
	cout << "\t" << help[i].cmdhelp << endl;
}

bool CmdLineParser::IsRegistered(const string& cmd)
{
    // checks id cmd is already registered
    CmdMap::const_iterator it;
    for (it=m_cmdmap.begin(); it!=m_cmdmap.end(); it++) {
	if (it->first == cmd)
	    return true;
    }
    return false;
}


void CmdLineParser::Parse(const string &line, StringVec &pv)
{
    pv.clear();
    if (line == "") {
	pv.clear();
	return;		// nothing to parse
    }

    // Trim beginning/ending spaces
    string::size_type i1, i2;
    i1 = line.find_first_not_of(" ");
    i2 = line.find_last_not_of(" ");
    if (i1 == string::npos) {
	// White-space line
	return;
    }

    string pline(line.substr(i1,i2-i1+1));

    // Remove tailing quote
    string::size_type s = pline.size();
    if (pline[s-1] == '\"')
	pline.erase(s-1);

    // Remove extra middle spaces
    i1 = 0;
    i2 = pline.find(" ",i1);
    while (i2 != string::npos) {
	// treat quotes
	if (pline[i1] == '\"') { // check for quotes
	    i1++;  // disconsider intial quote
	    i2 = pline.find('\"',i1);  // search for final quote
	    if (i2 == string::npos)
		break;  // final quote was not found
	}
	pv.push_back(pline.substr(i1,i2-i1));
	i1 = pline.find_first_not_of(" ",i2+1);
	i2 = pline.find(" ",i1);
    }
    pv.push_back(pline.substr(i1));
}

int  CmdLineParser::Parse(char* line, char* argv[])
{
    if (line == NULL)
	return 0;

    int i;

    argv[0] = line;
    i = 1;

    char *p;
    while ((p = strchr(argv[i-1],' ')) != NULL) {
	*p = '\0';
	argv[i++] = p+1;
    }
    argv[i] = NULL;

    return i;
}

string CmdLineParser::Help(const std::string& cmd)
{
    ostringstream os;
    if (m_cmdmap.count(cmd)<1)
	os << "Unknown command: " << cmd;
    else
	os << cmd << ": " << m_cmdmap[cmd].doc;
    return os.str();
}

string CmdLineParser::readline(const std::string& prompt)
{
    char* line = ::readline(prompt.c_str());
    if (line == NULL) {
	return string("");
	// throw EmptyLine();
    }
    
    char* cropped = crop(line);
    if (*cropped == '\0') {
	delete [] line;
	return string("");
	//throw EmptyLine();
    }
    string cmd(cropped);
    add_history(cropped);
    delete [] line;
    return cmd;
}

char** CmdLineParser::my_completion(const char* text, int start, int end)
{
    char** matches = (char**)NULL;
    if (start == 0)
	matches = rl_completion_matches(text,command_generator);
    return matches;
}

char* CmdLineParser::command_generator(const char* text, int state)
{
    static int len;
    static CmdMap::iterator list_index;
    const char* name;
	
    CmdLineParser* clp = Instance();
	
    if (state == 0) {
	list_index = clp->m_cmdmap.begin();
	len = strlen(text);
    }

    while (list_index != clp->m_cmdmap.end()) {
	if (list_index->first.substr(0,len) == text) {
	    char* cmd = dupstr(list_index->first.c_str());
	    list_index++;
	    return cmd;
	}
	list_index++;
    }
	
    // no matches
    return NULL;
}

Command* CmdLineParser::find_command(const string& name)
{
    if (m_cmdmap.count(name) < 1)
	return NULL;
    else
	return &(m_cmdmap[name]);
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
    for (pf=s+strlen(s)-1; pf>s && *pf==' '; pf--) ;
    *(pf+1) = '\0';
    return pi;
}
