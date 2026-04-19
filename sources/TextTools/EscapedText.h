//
// Created by Luiz Lima Jr. on 14/04/26.
//

#ifndef ESCAPE_DECO_ESCAPEDTEXT_H
#define ESCAPE_DECO_ESCAPEDTEXT_H

#include <string>
#include <initializer_list>
#include <vector>
#include <print>
#include "TextStyle.h"

namespace colibry {

	class NumSeq {
	public:
		NumSeq() = default;
		explicit NumSeq(uint8_t x) { append(x); }
		NumSeq(std::initializer_list<uint8_t> il) : numbers_{il} {}
		explicit NumSeq(const TextStyle& style);

		void append(uint8_t x) { numbers_.push_back(x); }
		void append(std::initializer_list<uint8_t> il);
		NumSeq& operator<<(uint8_t x) { append(x); return *this; }

		operator std::string() const;
	private:
		std::vector<uint8_t> numbers_;
	};

	class EscapedText {
	public:
		template <typename... Args>
		explicit EscapedText(const TextStyle& ts, Args&&... args)
			: txt_{std::forward<Args>(args)...}, eseq_{static_cast<std::string>(NumSeq{ts})} {}

		template <typename... Args>
		explicit EscapedText(const NumSeq& ns, Args&&... args)
			: txt_{std::forward<Args>(args)...}, eseq_{static_cast<std::string>(ns)} {}

		operator std::string() const;
	private:
		std::string txt_;
		std::string eseq_;
	};

} // end namespace

template<>
struct std::formatter<colibry::EscapedText> : std::formatter<std::string> {
	template<class Context>
	constexpr auto format(const colibry::EscapedText& t, Context& ctx) const {
		return format_to(ctx.out(), "{}", static_cast<std::string>(t));
	}
};

template <typename... Args>
void println(const colibry::TextStyle& ts, std::format_string<Args...> fmt, Args&&... args)
{
	std::print("\033[{}m", static_cast<std::string>(colibry::NumSeq{ts}));
	std::print(fmt, std::forward<Args>(args)...);
	std::println("\033[0m");
}

#endif //ESCAPE_DECO_ESCAPEDTEXT_H
