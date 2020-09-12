#include "OptionManager.h"
#include <cstring>

using namespace std;
using namespace colibry;

OptionManager::OptionManager(int argc, char* argv[])
	: m_argc{argc}, m_argv{argv}
{
	// parse and build map
	m_mandpar = nullptr;
	for (int i=1; i<argc; i++) {
		if (m_argv[i][0] == '-') {
			// option found
			const char* arg = m_argv[i+1];
			if (arg != nullptr)
				if (*arg == '-')
					arg = nullptr;
			if (m_argv[i][1] == '-')
				m_optmap[m_argv[i]+2] = arg;	// exclude "--"
			else
				m_optmap[m_argv[i]+1] = arg;	// exclude '-'
			if (arg != nullptr)
				++i;
		} else {
			// mandatory parameter (just one allowed)
			// if multiple, just the last is considered
			m_mandpar = argv[i];
		}
	}
}

const char* OptionManager::param(const string& o)
{
	if (m_optmap.count(o) > 0)
		return m_optmap[o];
	return nullptr;
}

bool OptionManager::has(const string& opt)
{
	return (m_optmap.count(opt)>0);
}
