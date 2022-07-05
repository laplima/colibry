//
// Reference for terminal colors:
// 	https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
//

#include <iostream>
#include <sstream>
#include <regex>
#include <sys/ioctl.h>
#include <iomanip>
#include "TextTools.h"

#define ESCSEQ (char)0x1b << '['

std::string colibry::ABSTAB(unsigned short t)
{
	std::ostringstream ss;
	ss << cursor_back(term_cols()) << ESCSEQ << t << 'C';
	return ss.str();
}

std::string colibry::SEPARATOR(unsigned short t)
{
	std::ostringstream ss;
	unsigned short cols = term_cols();
	if (t>cols || t==0)
		t = cols;
	for (auto i=0; i<t; ++i)
		ss << '-';
	return ss.str();
}

unsigned short colibry::term_cols()
{
	struct winsize ws{};
	if (ioctl(1,TIOCGWINSZ,&ws) < 0)
		return 0;
	return ws.ws_col;
}

unsigned short colibry::term_lines()
{
	struct winsize ws{};
	if (ioctl(1,TIOCGWINSZ,&ws) < 0)
		return 0;
	return ws.ws_row;
}

std::string colibry::cursor_back(unsigned short col)
{
	std::stringstream ss;
	ss << ESCSEQ << col << 'D';
	return ss.str();
}

std::string colibry::cursor_up(unsigned short lines)
{
	std::stringstream ss;
	ss << ESCSEQ << lines << 'A';
	return ss.str();
}

std::string colibry::erase_end_of_line()
{
	std::stringstream ss;
	ss << ESCSEQ << 'K';
	return ss.str();
}

std::string colibry::save_cursor()
{
	std::stringstream ss;
	ss << ESCSEQ << 's';
	return ss.str();
}

std::string colibry::restore_cursor()
{
	std::stringstream ss;
	ss << ESCSEQ << 'u';
	return ss.str();
}



std::string colibry::set_color(unsigned char tc, bool bold)
{
	tc = 30 + tc%8;	// make sure it belongs to a valid terminal color interval
	std::stringstream ss;
	ss << ESCSEQ << tc << (bold ? ";1" : "") << "m";
	return ss.str();
}

std::string colibry::set_color(unsigned char r, unsigned char g, unsigned char b, bool bold)
{
	std::stringstream ss;
	ss << ESCSEQ << "38;2;" << int(r) << ";" << int(g) << ";" << int(b) << (bold ? ";1" : "") << "m";
	return ss.str();
}

std::string colibry::set_color(const std::string& rgb, bool bold)
{
	// rgb = "#RRGGBB"
	int start = (rgb[0] == '#' ? 1 : 0);
	std::istringstream is{rgb.substr(start,2) + " " + rgb.substr(start+2,2) + " " + rgb.substr(start+4,2)};
	unsigned int r, g, b;
	is >> std::hex >> r >> g >> b;
	return set_color(r,g,b,bold);
}

std::string colibry::reset_color()
{
	std::stringstream ss;
	ss << ESCSEQ << "0m";
	return ss.str();
}

void colibry::cls()
{
	std::cout << ESCSEQ << "2J";
}

std::vector<std::string> split(const std::string& input, const std::string& regex)
{
	// passing -1 as the submatch index parameter performs splitting
	std::regex re{regex};
	std::sregex_token_iterator
		first{input.begin(), input.end(), re, -1},
		last;
	return {first, last};
}
