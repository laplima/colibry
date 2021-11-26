#include "Logger.h"
#include <string>
#include <colibry/DateTime.h>

using namespace std;
using namespace colibry;

string ToString(LogLevel level)
{
	switch (level) {
		case LogLevel::ERROR:
			return string("ERROR");
		case LogLevel::WARNING:
			return string("WARNING");
		case  LogLevel::INFO:
			return string("INFO");
		case  LogLevel::DEBUG0:
			return string("DEBUG-0");
		case  LogLevel::DEBUG1:
			return string("DEBUG-1");
		case  LogLevel::DEBUG2:
			return string("DEBUG-2");
		case  LogLevel::DEBUG3:
			return string("DEBUG-3");
	    }
}

ostringstream& Logger::get(LogLevel level)
{
	if (!s_disable_ts) {
	    DateTime now = DateTime::Now();
		m_ss << "[" << now << "] " << ToString(level) << ": ";
	}
	m_currlevel = level;
	//m_ss << string(level>DEBUG0?0:level-DEBUG0,'\t');
	return m_ss;
}

LogLevel Logger::s_reportinglevel = LogLevel::DEBUG0;
Logger::OutItem Logger::s_outmap[7] = {
    { LogLevel::ERROR, &cerr },
    { LogLevel::WARNING, &cerr },
    { LogLevel::INFO, &cout },
    { LogLevel::DEBUG0, &cout },
    { LogLevel::DEBUG1, &cout },
    { LogLevel::DEBUG2, &cout },
    { LogLevel::DEBUG3, &cout }
};

bool Logger::s_disable_ts = false;

LogLevel& Logger::ReportingLevel()
{
    return s_reportinglevel;
}

ostream*& Logger::Stream(LogLevel level)
{
	for (unsigned int i=0; i<7; i++)
		if (level == s_outmap[i].level)
			return s_outmap[i].os;
	return s_outmap[0].os;
}

Logger::~Logger()
{
    ostream* out = Stream(m_currlevel);
    (*out) << m_ss.str() << endl;
}

