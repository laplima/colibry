/*
//	CONSOLE APPLICATION - abstract class
//
//  (C) 2003-2006 by LAPLJ.
//  All rights reserved.
//
//  Create derived class MyApp overridding the following methods:
//
//  - Constructor (use one available for the base class)
//
//  - int Main(); [private]
//
//        Mandatory.
//
//  - void Init(); [private]
//
//        Optional. To initialize any data.
//
//  - void Terminate(); [private]
//
//        Optional. To destroy any data needed.
//
//  - bool ProcessCmdLineArgs() throw (WrongCmdLineArgsExc); [protected]
//
//        Optional. To process command line arguments. By default,
//        treats -h and -info commands. Return values: true:
//        something was done; false: nothing was done.
//        Replaces ProcessDefaultCmds that has been deprecated.
//
//  - void Help(); [protected]
//
//        Optional. Usage instructions.
//
//  - void About();
//  - void Info();
//
    When Run [public] is called, the folowing sequence is executed:

    1) Init();
    2) ProcessCmdLineArgs();
    3) Various checks (Date, TimesRun, Serial)
    4) Main();
    5) Terminate();

//
//  OS: UNIX / WINDOWS
*/


#ifndef CONSOLEAPP_H
#define CONSOLEAPP_H

#include <ctime>
#include <string>
#include <vector>
#include "Exception.h"
#include "DateTime.h"

namespace colibry {

    //
    // EXCEPTIONS
    //

    // Application use time expired (what: expire time)
    class ExpiredExc : public Exception {
    public:
		ExpiredExc(const std::string& when) : Exception(when) {}
		ExpiredExc(const std::string& when,
			   const std::string& inWhere) : Exception(when,inWhere) {}
    };

    // Application times of use exceeded limit (what: count limit)
    class RunoutExc : public Exception {
    public:
		RunoutExc(const std::string& limit) : Exception(limit) {}
		RunoutExc(const std::string& limit,
			  const std::string& inWhere) : Exception(limit,inWhere) {}
    };

    // Application serial # failed (what: message)
    class SerialNoExc : public Exception {
    public:
		SerialNoExc(const std::string& what) : Exception(what) {}
		SerialNoExc(const std::string& what,
			    const std::string& inWhere) : Exception(what,inWhere) {}
    };

    class WrongCmdLineArgsExc : public Exception {
    public:
		WrongCmdLineArgsExc(const std::string& what, const int exit_cd)
		    : Exception(what), exit_code(exit_cd) {}
		int exit_code;
    };

    class FatalExc : public Exception {
    public:
		FatalExc(const std::string& wht) : Exception(wht) {}
		FatalExc(const std::string& wht, const int type)
		    : Exception(wht,"",type) {}
		FatalExc(const std::string& wht, const std::string& whr, const int type)
		    : Exception(wht,whr,type) {}
    };

    //
    // TYPES
    //

    using ArgVec = std::vector<std::string>;

    //
    // CONSOLE APP CLASS
    //

    class ConsoleApp {
    public:

		ConsoleApp(int argc, char* argv[]);
		ConsoleApp(const ArgVec& args);
		ConsoleApp(const std::string &inAppName);
		virtual ~ConsoleApp();

		int Run();

		void SetTimesToRun(unsigned short ttr, const std::string &pfile);
		void SetExpireDate(const DateTime& ed);
		void SetExpireDate(const unsigned short ndays);

    protected:

		virtual bool ProcessCmdLineArgs();  // -h and -info

		virtual void Help();	// Help for command line args (USAGE)
		virtual void About();	// Version, limitations and serial #
		virtual void Info();	// Signature

    private:

		virtual void Init() {}
		virtual int  Main() = 0;  // to override
		virtual void Terminate() {}

		bool CheckSerialNo();   // TO DO
		bool CheckDate();	// Checks if expired
		bool CheckTimesRun();   // Checks # of times run

    protected:

		ArgVec m_args;          // STL version of argv
		int m_argc;             // original alrgc e argv
		char** m_argv;

		int m_exec_cnt;         // no. of executions before failing
		std::string m_exec_file;// file name of persistent storage of # of runs

		DateTime* m_expire;        // expiration date (if any)
    };

};    // namespace

#endif
