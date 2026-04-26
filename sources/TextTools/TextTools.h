#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include "EscapedText.h"

namespace colibry {

	// Fixed-length tabs
	const unsigned short DEFAULT_TAB = 40;

	std::string ABSTAB(unsigned short t=DEFAULT_TAB);
	std::string SEPARATOR(unsigned short t=0);	// 0 means: term_cols()

	unsigned short term_cols();
	unsigned short term_lines();

	constexpr unsigned short maxcursor = 1000;

	// special escape sequences
	std::string cursor_back(unsigned short col=maxcursor);
	std::string cursor_up(unsigned short lines=maxcursor);
	std::string erase_end_of_line();
	std::string save_cursor();
	std::string restore_cursor();

	std::string set_color(unsigned char tc, bool bold=false);	// 0<=tc<=7
	std::string set_color(unsigned char r, unsigned char g, unsigned char b, bool bold=false);
	std::string set_color(const std::string& rgb, bool bold=false);
	std::string reset_color();

	void cls();

	std::vector<std::string> split(const std::string& input,
		const std::string& regex = "\\s+");

	template <typename Range, typename Value = typename Range::value_type>
	std::string join(Range const& elements, const char *const delimiter)
	{
	    std::ostringstream os;
	    auto b = begin(elements);
	    auto e = end(elements);

	    if (b != e) {
	        std::copy(b, prev(e), std::ostream_iterator<Value>(os, delimiter));
	        b = prev(e);
	    }
	    if (b != e) {
	        os << *b;
	    }

	    return os.str();
	}

	// unicode manip
	void remove_accents(std::string& s);	// remove accents from s
	std::string::size_type unicode_size(const std::string_view& s);

}; // namespace

#endif
