//
// (C) LAPLJ 2012-2019
//
// Usage:
//
//		SET_REPORTING_LEVEL(DEBUG0);
//		LOG(ERROR) << "Erro!";
//		LOG(INFO) << "Info " << 123;
//
// Default output streams (levels):
//
//		ERROR	=> cerr
//		WARNING	=> cerr
//		INFO	=> cout
//		DEBUG0	=> cout
//		DEBUG1	=> cout
//		DEBUG2	=> cout
//		DEBUG3	=> cout
//
// Note: Previously, users had to specify LogLevel::INFO, etc. for the
// macros. This is no longer needed.
//

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>

namespace colibry {

	enum class LogLevel : char {
		ERROR,
		WARNING,
		INFO,
		DEBUG0,
		DEBUG1,
		DEBUG2,
		DEBUG3
	};

	class Logger {
	public:
		Logger() {}
		virtual ~Logger();
		std::ostringstream& get(LogLevel level=LogLevel::INFO);
	public:
		static LogLevel& ReportingLevel();
		static std::ostream*& Stream(LogLevel level);
		static inline void ToggleTimestamp() { s_disable_ts = !s_disable_ts; }
	protected:
		std::ostringstream m_ss;
		LogLevel m_currlevel;
	private:
		Logger(const Logger&) {}
		Logger& operator=(const Logger& l) { return *this; }
		static LogLevel s_reportinglevel;

		struct OutItem {
			LogLevel level;
			std::ostream* os;
		};
		static OutItem s_outmap[7];
		static bool s_disable_ts;		// disable timestamp? (default = false)
    }; // Logger class

#define SET_REPORTING_LEVEL(level)	Logger::ReportingLevel() = LogLevel::level
#define SET_REPORTING_LEVEL_(level)	Logger::ReportingLevel() = level

#define SET_OUTPUT_STREAM(level,os) Logger::Stream(LogLevel::level) = &os
#define SET_OUTPUT_STREAM_(level,os)	 Logger::Stream(level) = &os

#define SET_DEFAULT_STREAM(os)			 \
    Logger::Stream(LogLevel::ERROR) = &os; \
    Logger::Stream(LogLevel::WARNING) = &os; \
    Logger::Stream(LogLevel::INFO) = &os; \
    Logger::Stream(LogLevel::DEBUG0) = &os; \
    Logger::Stream(LogLevel::DEBUG1) = &os; \
    Logger::Stream(LogLevel::DEBUG2) = &os; \
    Logger::Stream(LogLevel::DEBUG3) = &os

#define LOG(level) \
	if (LogLevel::level>Logger::ReportingLevel()) ;	\
	else Logger().get(LogLevel::level)

#define LOG_(level) \
	if (level>Logger::ReportingLevel()) ;	\
	else Logger().get(level)

};     // namespace

#endif
