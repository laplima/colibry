// -------------------------------------------------
//
//   (FATAL) EXCEPTION CLASSES
//
//   Copyright (C) 1996-2017 by LAPLJ. All rights reserved.
//
// -------------------------------------------------

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>
#include <sstream>
#include <iostream>

// -------------------------------------------------
//  GENERAL EXCEPTION
// -------------------------------------------------

namespace colibry {

	#define ThrowIf(cond,msg) colibry::Exception::tif(cond,colibry::Exception::Formatter() << msg)

	class Exception : public std::exception {
	public:

		Exception() {
			mType = 0;
		}
		Exception(int inType) {
			mType = inType;
		}
		Exception(int inType, const std::string& inWhere) {
			mWhere = inWhere;
			mType = inType;
		}
		Exception(const std::string& inWhat) {
			mWhat = inWhat;
			mType = 0;
		}
		Exception(const std::string& inWhat, const std::string& inWhere,
			int inType=0) {
			mWhat = inWhat;
			mWhere = inWhere;
			mType = inType;
		}
		virtual ~Exception() throw() {};

		// Local (not to be overidden...)
		const char* what() const throw() { return mWhat.c_str(); }
		const char* where() const throw() { return mWhere.c_str(); }
		int type() const throw() { return mType; }

		friend std::ostream& operator<<(std::ostream& os, Exception& exc);

	protected:

		int mType;	     // Exception Type
		std::string mWhere;  // String indicating where the exception took place
		std::string mWhat;   // What happened

	public:

		template <typename E=colibry::Exception>
		static inline void tif(bool cond, const std::string& msg) { if (cond) throw E{msg}; }

	public:

		class Formatter {		// to allow messages in the form of streams: "ds" << 2 << ...
		public:
			Formatter() {}
			virtual ~Formatter() {}
			template <typename T>
			Formatter& operator<<(const T& s) { m_ss << s; return *this; }
			std::ostringstream& get() { return m_ss; }
			std::string str() const { return m_ss.str(); }
			operator std::string() const { return m_ss.str(); }
		private:
			std::ostringstream m_ss;
			Formatter(const Formatter&) = delete;
			Formatter& operator=(const Formatter&) = delete;
		};

	};

};  // namespace

#endif
