//
// DATETIME implementation
//
//     Copyrights (C) 2003 by LAPLJ.
//     All rights reserved.
//

#include <time.h>
#include <map>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <memory>
#include "DateTime.h"

using namespace std;
using namespace colibry;

const char* DTFORMAT = "%d/%m/%Y (%a) %H:%M:%S"; // see strftime()

//
// WEEKDAY MAP
//

class WeekdayMap : public map<string,int> {
public:
    WeekdayMap();
    string GetWeekdayName(int d);
};

WeekdayMap::WeekdayMap()
{
    (*this)["Sun"] = 0;
    (*this)["Mon"] = 1;
    (*this)["Tue"] = 2;
    (*this)["Wed"] = 3;
    (*this)["Thu"] = 4;
    (*this)["Fri"] = 5;
    (*this)["Sat"] = 6;
}

string WeekdayMap::GetWeekdayName(int d)
{
    for (iterator i = begin(); i != end(); i++)
	if (i->second == d)
	    return i->first;
    return string();
}

//
// MonthMap
//

struct MonthInfo {
    string name;
    int ndays;
};

class MonthMap : public map<int,MonthInfo> {
public:
    MonthMap();
    int GetMonthNo(const string& mname);
};

MonthMap::MonthMap()
{
    (*this)[1].name = "Jan";
    (*this)[1].ndays = 31;
    (*this)[2].name = "Feb";
    (*this)[2].ndays = 28;
    (*this)[3].name = "Mar";
    (*this)[3].ndays = 31;
    (*this)[4].name = "Apr";
    (*this)[4].ndays = 30;
    (*this)[5].name = "May";
    (*this)[5].ndays = 31;
    (*this)[6].name = "Jun";
    (*this)[6].ndays = 30;
    (*this)[7].name = "Jul";
    (*this)[7].ndays = 31;
    (*this)[8].name = "Aug";
    (*this)[8].ndays = 31;
    (*this)[9].name = "Sep";
    (*this)[9].ndays = 30;
    (*this)[10].name = "Oct";
    (*this)[10].ndays = 31;
    (*this)[11].name = "Nov";
    (*this)[11].ndays = 30;
    (*this)[12].name = "Dec";
    (*this)[12].ndays = 31;
}

int MonthMap::GetMonthNo(const string& m)
{
    for (iterator i=begin(); i!=end(); i++)
	if (i->second.name == m)
	    return i->first;
    return 0;
}

// GLOBALS

MonthMap   g_month_map;
WeekdayMap g_weekday_map;

//
// DATETIME IMPLEMENTATION
//

DateTime::DateTime()
{
    m_tm.tm_sec = 0;
    m_tm.tm_min = 0;
    m_tm.tm_hour = 0;
    m_tm.tm_mday = 1;
    m_tm.tm_mon = 0;
    m_tm.tm_year = 70;
    m_tm.tm_wday = 0;
    m_tm.tm_yday = 0;
    m_tm.tm_isdst = -1;
}

DateTime::DateTime(const int d, const int m, const int y)
{
    // hour is set to 23:59:59
    m_tm.tm_sec = 59;
    m_tm.tm_min = 59;
    m_tm.tm_hour = 23;
    m_tm.tm_mday = d;
    m_tm.tm_mon = m-1;
    m_tm.tm_year = (y>=1900) ? y-1900 : y;
    m_tm.tm_wday = 0;
    m_tm.tm_yday = 0;
    m_tm.tm_isdst = -1;

    mktime(&m_tm);      // adjust
}

DateTime::DateTime(const struct tm& tm_struct)
{
    *this = tm_struct;
}

DateTime::DateTime(const time_t& timep)
{
    *this = timep;
}

DateTime::DateTime(const DateTime& dt)
{
    *this = dt;
}

DateTime::DateTime(const string& fmt)
{
    *this = Parse(fmt);
}

DateTime::~DateTime()
{
}

void DateTime::SetTime(int h, int m, int s)
{
    m_tm.tm_sec = s;
    m_tm.tm_min = m;
    m_tm.tm_hour = h;

    mktime(&m_tm);   // adjust
}

DateTime& DateTime::operator=(const DateTime& dt)
{
    if (&dt != this)
	   *this = dt.m_tm;
    return *this;
}

DateTime& DateTime::operator=(const struct tm& tm_struct)
{
    m_tm.tm_sec = tm_struct.tm_sec;
    m_tm.tm_min = tm_struct.tm_min;
    m_tm.tm_hour = tm_struct.tm_hour;
    m_tm.tm_mday = tm_struct.tm_mday;
    m_tm.tm_mon = tm_struct.tm_mon;
    m_tm.tm_year = tm_struct.tm_year;
    m_tm.tm_wday = tm_struct.tm_wday;
    m_tm.tm_yday = tm_struct.tm_yday;
    m_tm.tm_isdst = tm_struct.tm_isdst;

    return *this;
}

DateTime& DateTime::operator=(const time_t& timep)
{
    struct tm *tms = localtime(&timep);

    m_tm.tm_sec = tms->tm_sec;
    m_tm.tm_min = tms->tm_min;
    m_tm.tm_hour = tms->tm_hour;
    m_tm.tm_mday = tms->tm_mday;
    m_tm.tm_mon = tms->tm_mon;
    m_tm.tm_year = tms->tm_year;
    m_tm.tm_wday = tms->tm_wday;
    m_tm.tm_yday = tms->tm_yday;
    m_tm.tm_isdst = tms->tm_isdst;

    return *this;
}

DateTime& DateTime::operator=(const string& fdate)
{
    *this = Parse(fdate);
    return *this;
}

DateTime& DateTime::operator+=(const unsigned short ndays)
{
    m_tm.tm_mday += ndays;
    mktime(&m_tm);
    return *this;
}

DateTime DateTime::operator+(const unsigned short ndays) const
{
    DateTime dt(*this);
    dt.m_tm.tm_mday += ndays;
    mktime(&(dt.m_tm));

    return dt;
}

time_t DateTime::operator-(const DateTime& dt)
{
    DateTime aux(dt);
    time_t t0 = (time_t)(*this);
    time_t t1 = (time_t)aux;
    return t0-t1;
}

bool DateTime::operator<(const DateTime& dt)
{
    time_t this_time, that_time;
    this_time = mktime(&m_tm);
    struct tm t = dt.m_tm;
    that_time = mktime(&t);
    return (bool)(this_time < that_time);
}

bool DateTime::operator>(const DateTime& dt)
{
    time_t this_time, that_time;
    this_time = mktime(&m_tm);
    struct tm t = dt.m_tm;
    that_time = mktime(&t);
    return (bool)(this_time > that_time);
}

bool DateTime::operator==(const DateTime& dt)
{
    time_t this_time, that_time;
    this_time = mktime(&m_tm);
    struct tm t = dt.m_tm;
    that_time = mktime(&t);
    return (bool)(this_time == that_time);
}

bool DateTime::operator!=(const DateTime& dt)
{
    return !(*this == dt);
}

bool DateTime::operator<=(const DateTime& dt)
{
    return !(*this > dt);
}

bool DateTime::operator>=(const DateTime& dt)
{
    return !(*this < dt);
}

DateTime::operator time_t()
{
    return mktime(&m_tm);
}

DateTime::operator const char*() const
{
    return to_ctime();
}

DateTime::operator string() const
{
    return to_str();
}

bool DateTime::IsSameDay(const DateTime& dt)
{
    return (bool)((day() == dt.day()) && (month() == dt.month())
		  && (year() == dt.year()));
}

bool DateTime::IsToday()
{
    return IsSameDay(Now());
}

string DateTime::Format(const char* format) const
{
    // check strftime manuals for syntax
    char s[512];
    strftime(s,512,format,&m_tm);
    string t(s);
    return t;
}

//
//  STATIC FUNCTIONS
//

DateTime DateTime::Now()
{
    time_t now = ::time(nullptr);
    struct tm* t = localtime(&now);
    DateTime nowd(*t);

    return nowd;
}

DateTime DateTime::Parse(const string& s, const char* format)
{
    if (format == nullptr)
	format = DTFORMAT;   // default format

    string copy(s);
    crop(copy);
    DateTime d;

    if (strptime(copy.c_str(),format,&(d.m_tm))==nullptr)
	cerr << "Parse(): wrong date format" << endl;
    else
	mktime(&(d.m_tm));      // adjust

    /*
      string::size_type pd = copy.find(" ");
    string::size_type ph = copy.rfind(" ");
    if (pd == string::npos || ph == string::npos)
	cerr << "Parse(): wrong date format" << endl;
    else {
	pd++;
	d.m_tm.tm_wday = g_weekday_map[copy.substr(0,3)];
	d.m_tm.tm_mday = ::atoi(copy.substr(pd,2).c_str());
	d.m_tm.tm_mon = ::atoi(copy.substr(pd+3,2).c_str()) - 1;
	d.m_tm.tm_year = ::atoi(copy.substr(pd+6,4).c_str()) - 1900;

	ph++;
	d.m_tm.tm_hour = ::atoi(copy.substr(ph,2).c_str());
	d.m_tm.tm_min = ::atoi(copy.substr(ph+3,2).c_str());
	d.m_tm.tm_sec = ::atoi(copy.substr(ph+6,2).c_str());

	mktime(&(d.m_tm));   // adjust?
	} */
    return d;
}


DateTime DateTime::ParseDMY(const string& sdt)
{
    DateTime d(Parse(sdt,"%d/%m/%Y"));
    d.m_tm.tm_sec = 59;
    d.m_tm.tm_min = 59;
    d.m_tm.tm_hour = 23;
    return d;

    /*
    // parse string in the format DD/MM/YYYY
    DateTime d;
    string::size_type pos1 = sdt.find("/");
    string::size_type pos2 = sdt.rfind("/");

    if (pos1 == string::npos || pos2 == string::npos || pos1 == pos2) {
	cerr << "ParseDMY(): wrong date format." << endl;
	return d;
    }
    d.m_tm.tm_mday = ::atoi(sdt.substr(0,pos1).c_str());
    d.m_tm.tm_mon = ::atoi(sdt.substr(pos1+1,2).c_str()) - 1;
    d.m_tm.tm_year  = ::atoi(sdt.substr(pos2+1,4).c_str()) - 1900;
    // hour is set to 23:59:59

    mktime(&(d.m_tm));   // adjust

    return d;
    */
}

DateTime DateTime::ParseCTime(const string& ct)
{
    // Assumes ct is in correct format
    DateTime d;
    string cp(ct);
    crop(cp);

    d.m_tm.tm_mday = ::atoi(cp.substr(8,2).c_str());
    d.m_tm.tm_mon = g_month_map.GetMonthNo(cp.substr(4,3)) - 1;
    d.m_tm.tm_year = ::atoi(cp.substr(cp.length()-4,4).c_str()) - 1900;
    d.m_tm.tm_hour = ::atoi(cp.substr(11,2).c_str());
    d.m_tm.tm_min = ::atoi(cp.substr(14,2).c_str());
    d.m_tm.tm_sec = ::atoi(cp.substr(17,2).c_str());
    d.m_tm.tm_wday = g_weekday_map[cp.substr(0,3)];

    return d;
}

const string DateTime::to_str(const char* fmt) const
{
    if (fmt==nullptr)
	fmt = DTFORMAT;
    return Format(fmt);
}

const char* DateTime::to_ctime() const
{
    struct tm aux = m_tm;   // needs aux for mktime may modify m_tm
    time_t t = mktime(&aux);
    return ctime(&t);
}

void DateTime::crop(string& s)
{
    string::size_type p;
    p = s.find_first_not_of(" \t\n\r");
    s = s.substr(p);
    p = s.find_last_not_of(" \t\n\r");
    s = s.substr(0,p+1);
}

namespace colibry {

    ostream& operator<<(ostream& os, const DateTime& dt)
    {
	// WDAY DD/MM/YYYY HH:MM:SS
	return (os << dt.to_str());
	/*
	os << g_weekday_map.GetWeekdayName(dt.weekday()) << " "
	   << setfill('0') << setw(2) << dt.day() << "/"
	   << setfill('0') << setw(2) << dt.month() << "/"
	   << setw(4) << dt.year() << " "
	   << setfill('0') << setw(2) << dt.hour() << ":"
	   << setfill('0') << setw(2) << dt.min() << ":"
	   << setfill('0') << setw(2) << dt.sec();
	   return os;
	*/
    }

}
