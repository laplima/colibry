//
// adapted from libfmt
// (https://github.com/fmtlib/fmt/blob/main/include/fmt/color.h)
//

#ifndef TEXTSTYLE_H
#define TEXTSTYLE_H

#include "colors.h"
#include <utility>
#include <stdexcept>

namespace colibry {

	enum class emphasis : uint8_t {
		bold = 1,
		faint = 1 << 1,
		italic = 1 << 2,
		underline = 1 << 3,
		blink = 1 << 4,
		reverse = 1 << 5,
		conceal = 1 << 6,
		strikethrough = 1 << 7,
	};

	struct RGB {
		constexpr RGB() : r{0}, g{0}, b{0} {}
		constexpr RGB(uint8_t r_, uint8_t g_, uint8_t b_) : r{r_}, g{g_}, b{b_} {}
		constexpr RGB(uint32_t hex)
			: r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF) {}
		constexpr RGB(color hex)
			: r((static_cast<uint32_t>(hex) >> 16) & 0xFF),
			g((static_cast<uint32_t>(hex) >> 8) & 0xFF),
			b(static_cast<uint32_t>(hex) & 0xFF) {}

		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	// A bit-packed variant of an RGB color, a terminal color, or unset color.
	// see TextStyle for the bit-packing scheme.
	class ColorType {
	public:
		// constructors
		constexpr ColorType() noexcept = default;
		constexpr ColorType(color c) noexcept
			: value_(std::to_underlying(c) | (1 << 24)) {}
		constexpr ColorType(const RGB& rgb_color) noexcept
			: ColorType(static_cast<color>(
				(static_cast<uint32_t>(rgb_color.r) << 16) |
				(static_cast<uint32_t>(rgb_color.g) << 8) | rgb_color.b)) {}
		constexpr ColorType(terminal_color term_color) noexcept
			: value_(std::to_underlying(term_color) | (3 << 24)) {}
		explicit constexpr ColorType(uint32_t value) noexcept : value_{value}
			{} // raw value, with discriminator

		[[nodiscard]] constexpr bool is_terminal_color() const noexcept { return (value_ & (1 << 25)) != 0; }
		[[nodiscard]] constexpr uint32_t value() const noexcept { return value_ & 0xFFFFFF; } // remove discriminator

		ColorType& operator=(color c) noexcept { return *this = ColorType{c}; }
		ColorType& operator=(const RGB& rgb_color) noexcept { return *this = ColorType{rgb_color}; }
		ColorType& operator=(terminal_color term_color) noexcept { return *this = ColorType{term_color}; }
		ColorType& operator=(uint32_t v) noexcept { return (*this = ColorType{v}); }

		[[nodiscard]] constexpr uint32_t raw_value() const { return value_; }
	private:
		uint32_t value_ = 0;
	};

	// A text style consisting of foreground and background colors and emphasis.

	class TextStyle {
		// The information is packed as follows:
		// ┌──┐
		// │ 0│─┐
		// │..│ ├── foreground color value
		// │23│─┘
		// ├──┤
		// │24│─┬── discriminator for the above value. 00 if unset, 01 if it's
		// │25│─┘   an RGB color, or 11 if it's a terminal color (10 is unused)
		// ├──┤
		// │26│──── overflow bit, always zero (see below)
		// ├──┤
		// │27│─┐
		// │..│ │
		// │50│ │
		// ├──┤ │
		// │51│ ├── background color (same format as the foreground color)
		// │52│ │
		// ├──┤ │
		// │53│─┘
		// ├──┤
		// │54│─┐
		// │..│ ├── emphases
		// │61│─┘
		// ├──┤
		// │62│─┬── unused
		// │63│─┘
		// └──┘
		// The overflow bits are there to make operator|= efficient.
		// When ORing, we must throw if, for either the foreground or background,
		// one style specifies a terminal color and the other specifies any color
		// (terminal or RGB); in other words, if one discriminator is 11 and the
		// other is 11 or 01.
		//
		// We do that check by adding the styles. Consider what adding does to each
		// possible pair of discriminators:
		//    00 + 00 = 000
		//    01 + 00 = 001
		//    11 + 00 = 011
		//    01 + 01 = 010
		//    11 + 01 = 100 (!!)
		//    11 + 11 = 110 (!!)
		// In the last two cases, the ones we want to catch, the third bit——the
		// overflow bit——is set. Bingo.
		//
		// We must take into account the possible carry bit from the bits
		// before the discriminator. The only potentially problematic case is
		// 11 + 00 = 011 (a carry bit would make it 100, not good!), but a carry
		// bit is impossible in that case, because 00 (unset color) means the
		// 24 bits that precede the discriminator are all zero.
		//
		// This test can be applied to both colors simultaneously.
	public:

		constexpr TextStyle(emphasis em = emphasis()) noexcept
			: style_(static_cast<uint64_t>(em) << 54) {}
		explicit constexpr TextStyle(ColorType fgc) noexcept
			: TextStyle{fgc.raw_value()} {}
		explicit constexpr TextStyle(ColorType fgc, ColorType bgc) noexcept
			: style_(fgc.raw_value() | (bgc.raw_value() << 27)) {}

		constexpr TextStyle& operator|=(TextStyle rhs)
		{
			if (((style_ + rhs.style_) & ((1ULL << 26) | (1ULL << 53))) != 0)
				throw std::runtime_error{"can't OR a terminal color"};
			style_ |= rhs.style_;
			return *this;
		}

		friend TextStyle operator|(TextStyle lhs, TextStyle rhs) { return lhs |= rhs; }

		constexpr bool operator==(TextStyle rhs) const noexcept { return style_ == rhs.style_; }
		constexpr bool operator!=(TextStyle rhs) const noexcept { return !(*this == rhs); }

		[[nodiscard]] constexpr bool has_foreground() const noexcept { return (style_ & (1 << 24)) != 0; }
		[[nodiscard]] constexpr bool has_background() const noexcept { return (style_ & (1ULL << 51)) != 0; }
		[[nodiscard]] constexpr bool has_emphasis() const noexcept { return (style_ >> 54) != 0; };
		[[nodiscard]] constexpr ColorType get_foreground() const noexcept
		{
			// if (!has_foreground())
			// 	std::println(stderr, "no foreground specified for this style");
			return ColorType{static_cast<uint32_t>(style_ & 0x3FFFFFF)};
		}
		[[nodiscard]] constexpr ColorType get_background() const noexcept
		{
			// if(!has_background())
			// 	std::println(stderr, "no background specified for this style");
			return ColorType{static_cast<uint32_t>((style_ >> 27) & 0x3FFFFFF)};
		}

		[[nodiscard]] constexpr bool has_emphasis(emphasis e) const noexcept {
			return ((style_ >> 54) & std::to_underlying(e)) != 0; }

		[[nodiscard]] auto raw() const { return style_; }	// debug

	private:

		explicit constexpr TextStyle(uint64_t style) noexcept : style_{style} {}

		friend constexpr TextStyle fg(ColorType foreground) noexcept;
		friend constexpr TextStyle bg(ColorType background) noexcept;

		uint64_t style_ = 0;
	};

	/// Creates a text style from the foreground (text) color.
	constexpr  TextStyle fg(ColorType foreground) noexcept { return TextStyle{foreground.raw_value()}; }
	/// Creates a text style from the background color.
	constexpr  TextStyle bg(ColorType background) noexcept { return TextStyle{static_cast<uint64_t>(background.raw_value()) << 27}; }

} // end namespace

inline colibry::TextStyle operator|(colibry::emphasis lhs, colibry::emphasis rhs) noexcept
{
	return colibry::TextStyle{lhs} | colibry::TextStyle{rhs};
}

#endif
