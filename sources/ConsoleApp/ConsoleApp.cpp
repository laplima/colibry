#include "ConsoleApp.h"
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <iomanip>
#include <memory>

using namespace std;

//
// Helpers
//

bool WriteAppData(fstream* file, const unsigned short max,
				  const unsigned short curr, 
				  const colibry::DateTime* date=nullptr)
{
    file->clear();
    file->seekp(0L,ios::beg);
    *file << "CPEX*";
    file->write((char*)&curr,sizeof(curr));    // curr # of executions
    *file << "-1543-9F5E-7A87-9AF7/";
    file->write((char*)&max,sizeof(max));      // max # of executions
    if (date == nullptr)
		*file << "NEVER" << endl;
    else
		file->write((char*)date,sizeof(*date));
    return true;
}

bool ReadAppData(fstream* file, unsigned short* max,
				 unsigned short* curr, colibry::DateTime** date)
{
    file->clear();
    file->seekp(5L,ios::beg);
    file->read((char*)curr,sizeof(*curr));
    file->seekp(21L,ios::cur);
    file->read((char*)max,sizeof(*max));
    char buf[6];
    file->read(buf,5);
    buf[5] = '\0';
    string s(buf);
    if (s == "NEVER")
		*date = nullptr;
    else {
		*date = new colibry::DateTime();
		file->read((char*)*date,sizeof(**date));
    }
    return true;
}

//
// CCONSOLEAPP
//

colibry::ConsoleApp::ConsoleApp(int argc, char* argv[])
	: m_exec_cnt(-1), m_expire(nullptr)
{
    for (int i=0; i<argc; i++)
		m_args.push_back(argv[i]);
    m_argc = argc;
    m_argv = argv;
}

colibry::ConsoleApp::ConsoleApp(const ArgVec& args)
	: m_args(args), 
	m_argc(args.size()), 
	m_argv(nullptr), 
	m_exec_cnt(-1), 
	m_expire(nullptr)
{
}

colibry::ConsoleApp::ConsoleApp(const string &inAppName)
	: m_exec_cnt(-1), m_expire(nullptr)
{
    m_args.push_back(inAppName);
}

colibry::ConsoleApp::~ConsoleApp()
{
    if (m_expire != nullptr)
	delete m_expire;
}

void colibry::ConsoleApp::SetExpireDate(const DateTime& ed)
{
    if (m_expire != nullptr)
		delete m_expire;

    m_expire = new DateTime(ed);
}

void colibry::ConsoleApp::SetTimesToRun(unsigned short ttr, const string &pfile)
{
    const string TDIR("/tmp/");  // Directory to hold persistent # of runs

    m_exec_cnt = ttr;
    m_exec_file = TDIR + pfile;
}

int colibry::ConsoleApp::Run()
{
    int exit_code{};

    Init();

    try {

	ProcessCmdLineArgs();

	if (!CheckDate()) {
	    // expired
	    ostringstream what, where;
	    what << "Expired in " << *m_expire;
	    where << typeid(*this).name()+1 << "::Run()";
	    throw ExpiredExc(what.str(),where.str());
	}

	if (!CheckTimesRun()) {
	    // runout
	    ostringstream ss,where;
	    ss << "More than " << m_exec_cnt << " executions. ";
	    where << typeid(*this).name()+1 << "::Run()";
	    throw RunoutExc(ss.str(),where.str());
	}

	if (!CheckSerialNo()) {
	    // serial # error
	    ostringstream where;
	    where << typeid(*this).name()+1 << "::Run()";
	    throw SerialNoExc("Serial # error",where.str());
	}

	exit_code = Main();

    } catch(const WrongCmdLineArgsExc& e) {
	exit_code = e.exit_code;
    } catch (...) {
	throw; // propagates Main exceptions
    }

    Terminate();

    return exit_code;
}

bool colibry::ConsoleApp::CheckDate()
{
    if (m_expire == nullptr)
		return true;      // never expires

    return (*m_expire >= DateTime::Now());
}

bool colibry::ConsoleApp::CheckTimesRun()
{
    if (m_exec_cnt < 0)
	return true;   // No check required

    unsigned short curr_ex, max_ex;

    unique_ptr<fstream> theFile(new fstream (m_exec_file.c_str(),
					   ios::in|ios::out|ios::binary/*|ios::nocreate*/));
    if (!(*theFile)) {
	// Create new file
	theFile.reset(new fstream(m_exec_file.c_str(),
				  ios::in|ios::out|ios::binary));
	if (!(*theFile)) {
	    // cerr << "Times check failed..." << endl;
	    return false;
	}
	curr_ex = 0;
	max_ex = m_exec_cnt;

	WriteAppData(theFile.get(),max_ex,curr_ex);
    } else {
	// Use existing file
	DateTime *date;
	ReadAppData(theFile.get(),&max_ex,&curr_ex,&date);
	m_exec_cnt = max_ex;
    }

    curr_ex++;     // this is one execution

    // Check executions
    if (curr_ex > max_ex) {
	// over
	theFile->close();
	return false;
    }

    cerr << "[Times left: " << max_ex - curr_ex << "]" << endl;

    // Update file
    WriteAppData(theFile.get(),max_ex,curr_ex);

    theFile->close();

    return true;
}


bool colibry::ConsoleApp::CheckSerialNo()
{
    // TO DO

    return true;
}

void colibry::ConsoleApp::Help()
{
    if (m_args.size() > 0)
	cout << "USAGE: " << m_args[0] << "." << endl;
}

void colibry::ConsoleApp::About()
{
    cout << "About [" << m_args[0] << "]... (C) LAPLJ 2003. Version 1.0" << endl;
}

void colibry::ConsoleApp::Info()
{
    cout << "Info" << endl;
    if (m_expire != nullptr)
	cout << "Expire date: " << *m_expire << endl;
}

// Called BEFORE CheckDate(), CheckTimesRun(), CheckSerialNo() and  Main().
// If return value is true, the application exists (return code 0) without
// calling the check functions and Main().

bool colibry::ConsoleApp::ProcessCmdLineArgs()
{
    for (int i=1; i<m_args.size(); i++) {
	if (m_args[i] == "-h" || m_args[i] == "-help") {
	    Help();
	    throw WrongCmdLineArgsExc("Help",0);
	} else if (m_args[i] == "-info") {
	    Info();
	    throw WrongCmdLineArgsExc("Info",0);
	}
    }

    return false;  // nothing done
}
