// -------------------------------------------------
//
//   (FATAL) EXCEPTION CLASSES
//
//   Copyright (C) 1996-2017 by LAPLJ. All rights reserved.
//
// -------------------------------------------------

#ifndef EXCEPTION_H
#define EXCEPTION_H

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

		Exception() = default;
		explicit Exception(int inType) : mType{inType} {}
		Exception(int inType, std::string inWhere)
			: mType{inType}, mWhere{std::move(inWhere)} {}
		Exception(std::string inWhat) : mWhat{std::move(inWhat)} {}
		Exception(std::string inWhat, std::string inWhere, int inType=0)
			: mType{inType}, mWhere{inWhere}, mWhat{inWhat} {}

		// Local (not to be overidden...)
		[[nodiscard]] const char* what() const noexcept override { return mWhat.c_str(); }
		[[nodiscard]] const char* where() const noexcept { return mWhere.c_str(); }
		[[nodiscard]] int type() const noexcept { return mType; }

		friend std::ostream& operator<<(std::ostream& os, Exception& exc);

	protected:

		int mType = 0;	     // Exception Type
		std::string mWhere;  // String indicating where the exception took place
		std::string mWhat;   // What happened

	public:

		template <typename E=colibry::Exception>
		static inline void tif(bool cond, const std::string& msg) { if (cond) throw E{msg}; }

	public:

		class Formatter {		// to allow messages in the form of streams: "ds" << 2 << ...
		public:
			Formatter() = default;
			Formatter(const Formatter&) = delete;
			Formatter& operator=(const Formatter&) = delete;
			virtual ~Formatter() = default;
			template <typename T>
			Formatter& operator<<(const T& s) { m_ss << s; return *this; }
			std::ostringstream& get() { return m_ss; }
			std::string str() const { return m_ss.str(); }
			operator std::string() const { return m_ss.str(); }
		private:
			std::ostringstream m_ss;
		};

	};

};  // namespace

#endif
