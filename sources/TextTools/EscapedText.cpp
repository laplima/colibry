//
// Created by Luiz Lima Jr. on 14/04/26.
//

#include "EscapedText.h"
#include <string_view>

using namespace colibry;

// NumSeq

void NumSeq::append(std::initializer_list<uint8_t> il)
{
	numbers_.insert(numbers_.end(), il.begin(), il.end());
}

NumSeq::operator std::string() const
{
	std::string s;
	if (!numbers_.empty()) {
		s = std::to_string(numbers_[0]);
		for (int i=1; i<numbers_.size(); ++i)
			s += ";" + std::to_string(numbers_[i]);
	}
	return s;
}

// EscapedText

EscapedText::operator std::string() const
{
	return eseq(eseq_) + txt_ + eseq("0");
}

NumSeq::NumSeq(const TextStyle& style)
{
	// see https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_parameters
	// for the mapping of style to ANSI codes

	if (style.has_emphasis()) {
		if (style.has_emphasis(emphasis::bold))
			append(1);
		if (style.has_emphasis(emphasis::faint))
			append(2);
		if (style.has_emphasis(emphasis::italic))
			append(3);
		if (style.has_emphasis(emphasis::underline))
			append(4);
		if (style.has_emphasis(emphasis::blink))
			append(5);
		if (style.has_emphasis(emphasis::reverse))
			append(7);
		if (style.has_emphasis(emphasis::conceal))
			append(8);
		if (style.has_emphasis(emphasis::strikethrough))
			append(9);
	}

	if (style.has_foreground()) {
		auto fg = style.get_foreground();
		if (fg.is_terminal_color())
			append(fg.value());
		else {
			RGB r{fg.value()};
			append({38, 2, r.r, r.g, r.b});
		}
	}

	if (style.has_background()) {
		auto bg = style.get_background();
		if (bg.is_terminal_color())
			append(bg.value() + 10U);
		else {
			RGB r{bg.value()};
			append({48, 2, r.r, r.g, r.b});
		}
	}
}
