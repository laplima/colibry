#include "Logger.h"
#include <string>
#include <DateTime.h>

using namespace std;
using namespace colibry;

string ToString(LogLevel level)
{
	switch (level) {
		case LogLevel::ERROR:
			return {"ERROR"};
		case LogLevel::WARNING:
			return {"WARNING"};
		case  LogLevel::INFO:
			return {"INFO"};
		case  LogLevel::DEBUG0:
			return {"DEBUG-0"};
		case  LogLevel::DEBUG1:
			return {"DEBUG-1"};
		case  LogLevel::DEBUG2:
			return {"DEBUG-2"};
		case  LogLevel::DEBUG3:
			return {"DEBUG-3"};
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
std::array<Logger::OutItem,7> Logger::s_outmap = {
    OutItem{ .level=LogLevel::ERROR, .os=&cerr },
    OutItem{ .level=LogLevel::WARNING, .os=&cerr },
    OutItem{ .level=LogLevel::INFO, .os=&cout },
    OutItem{ .level=LogLevel::DEBUG0, .os=&cout },
    OutItem{ .level=LogLevel::DEBUG1, .os=&cout },
    OutItem{ .level=LogLevel::DEBUG2, .os=&cout },
    OutItem{ .level=LogLevel::DEBUG3, .os=&cout }
};

bool Logger::s_disable_ts = false;

LogLevel& Logger::ReportingLevel()
{
    return s_reportinglevel;
}

ostream*& Logger::Stream(LogLevel level)
{
	for (auto& om : s_outmap)
		if (level == om.level)
			return om.os;
	return s_outmap[0].os;
}

Logger::~Logger()
{
    ostream* out = Stream(m_currlevel);
    (*out) << m_ss.str() << '\n';
}
