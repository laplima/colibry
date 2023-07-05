//
// throw_if
// (C) 2018 by laplima
//
// Formatter allows use function like this:
// assert_throw(cond,Formatter() << s << x << y);
//
// Typical usage:
//
//	throw_if(x<0,"x = " << x);
//

#ifndef THROW_IF_H
#define THROW_IF_H

#include <string>
#include <stdexcept>
#include <sstream>

namespace colibry {

	class Formatter {
	public:
		Formatter() = default;
		virtual ~Formatter() = default;
		Formatter(const Formatter&) = delete;
		Formatter& operator=(const Formatter&) = delete;
		template<typename Type>
		Formatter& operator<<(const Type& s) { m_ss << s; return *this; }
		std::ostringstream& get() { return m_ss; }
		std::string str() const { return m_ss.str(); }
		operator std::string() const { return m_ss.str(); }
	private:
		std::ostringstream m_ss;
	};


	template <typename E = std::runtime_error>
	inline void assert_throw(bool cond, const std::string& what)
	{
		if (!cond) throw E{what};
	}

	template <typename E = std::runtime_error>
	inline void throwif(bool cond, const std::string& what)
	{
		if (cond) throw E{what};
	}

	#define throw_if(cond,msg) colibry::throwif(cond,colibry::Formatter() << msg)
};

#endif
