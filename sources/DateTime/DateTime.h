//
// DATETIME
//
//    Copyright (C) 2003-2012 by LAPLJ.
//    All rights reserved.
//
//  Default string format is defined by constant DTFORMAT (see DateTime.cpp)
//  and is:
//
//  DD/MM/YYYY (WDY) HH:MM:SS
//

#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <string>
#include <iostream>
#include <time.h>

namespace colibry {

	enum Weekday { SUN, MON, TUE, WED, THU, FRI, SAT };

	class DateTime {
	public:
		DateTime();
		DateTime(const int d, const int m, const int y); // time is set to 23:59
		DateTime(const struct tm& tm_struct);
		DateTime(const time_t& time);
		DateTime(const DateTime& dt);
		DateTime(const std::string& fmt);                // see DTFORMAT
		virtual ~DateTime();

		// Modifiers

		void SetTime(int h, int m, int s);               // change time
		void day(int d) { m_tm.tm_mday = d; }
		void month(int m) { m_tm.tm_mon = m-1; }
		void year(int y) { m_tm.tm_year = y-1900; }
		void hour(int h) { m_tm.tm_hour = h; }
		void min(int m) { m_tm.tm_min = m; }
		void sec(int s) { m_tm.tm_sec = s; }
		void weekday(int w) { m_tm.tm_wday = w; }

		// Operators

		DateTime& operator=(const DateTime& dt);
		DateTime& operator=(const struct tm& tm_struct);
		DateTime& operator=(const time_t& timep);
		DateTime& operator=(const std::string& fdate);   // see DTFORMAT
		DateTime& operator+=(const unsigned short ndays);
		DateTime operator+(const unsigned short ndays) const;
		time_t operator-(const DateTime& dt);
		bool operator>(const DateTime& dt);
		bool operator<(const DateTime& dt);
		bool operator==(const DateTime& dt);
		bool operator!=(const DateTime& dt);
		bool operator<=(const DateTime& dt);
		bool operator>=(const DateTime& dt);

		operator time_t();
		operator const char* () const;                  // same as to_ctime()
		operator std::string () const;                  // same as to_str()

		bool IsSameDay(const DateTime& dt);
		bool IsToday();

		// Format conversion

		const std::string to_str(const char* fmt=nullptr) const; // see DTFORMAT
		const char* to_ctime() const;
		std::string Format(const char* format) const;    // see strftime()

		// Selectors

		int day() const     { return m_tm.tm_mday; }
		int month() const   { return m_tm.tm_mon+1; }
		int year() const    { return m_tm.tm_year+1900; }
		int hour() const    { return m_tm.tm_hour; }
		int min() const     { return m_tm.tm_min; }
		int sec() const     { return m_tm.tm_sec; }
		int weekday() const { return m_tm.tm_wday; }

		// Stream output
		// DAY DD/MM/YYYY HH:MM:SS
		friend std::ostream& operator<<(std::ostream& os, const DateTime& dt);

	public:

		static DateTime Now();
		static DateTime Parse(const std::string& s, const char* format=nullptr);
		static DateTime ParseDMY(const std::string& sdt);  // DD/MM/YYYY
		static DateTime ParseCTime(const std::string& ct);

	private:

		struct tm m_tm;
		static void crop(std::string& s);
	};

};  // namespace

#endif
