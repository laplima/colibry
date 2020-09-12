//
// (Simple) OptionManager
// (C) LAPLJ
//
// Build map:
//		option : parameter (may be nullptr)
//
//	option is stripped of initial '-' or '--'
//
// USAGE:
//
//	OptionManager opt{argc, argv};
//
//	const char* p1 = opt.param("f");		// -f <p1>
//	const char* p2 = opt.paran("file");		// --file <p2>
//	if (opt.has("help")) {} 				// --help
//

#ifndef __OPTIONMANAGER_H__
#define __OPTIONMANAGER_H__

#include <map>
#include <string>

namespace colibry {

	class OptionManager {
	// public:
	// 	static const int UNKNOWN;
	public:
		OptionManager(int argc, char* argv[]);
		const char* param(const std::string& opt);
		const char* mandatory() { return m_mandpar; }
		bool has(const std::string& opt);  // parameter-less option
	private:
		int     m_argc;
		char**	m_argv;
		std::map<std::string,const char*> m_optmap; // option => argv[i]
		const char* m_mandpar;  // mandatory parameter (just one)
	};
};

#endif
