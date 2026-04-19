#include <print>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <colibry/Property.h>

using namespace std;
using namespace colibry;

struct Position {
	int x,y;
	Position(int a, int b) : x(a), y(b) {}
	Position() : x{0}, y{0} {}
};

template<>
struct std::formatter<Position> : std::formatter<string> {
	template<class Context>
	constexpr auto format(const Position& p, Context& ctx) const {
		return format_to(ctx.out(), "({},{})", p.x, p.y);
	}
};

class Test {
public:
	Test() : count{this,&Test::get,&Test::set}, pos{this,&Test::getp,&Test::setp} {}

	[[nodiscard]] int get() const { println("Test::gcount()"); return m_count+1; }
	void set(int x) { println("Test::scount()"); m_count = x; }

	[[nodiscard]] Position getp() const { println("Test::gpos()"); return m_pos; }
	void setp(const Position p) { println("Test::spos()"); m_pos = p; }

	Property<Test,int,PropType::rdwr> count;
	Property<Test,Position,PropType::rdwr> pos;
private:
	int m_count = 0;
	Position m_pos;
};

class TestStr {
public:
	TestStr()
	{
		Txt.setContainer(this);
		Txt.setter(&TestStr::setTxt);
		Txt.getter(&TestStr::getTxt);
	}

	void setTxt(const char* x) { println("TestStr::sTxt"); m_txt = x; }
	[[nodiscard]] const char* getTxt() const { println("TestStr::gTxt"); return m_txt.c_str(); }

	Property<TestStr,const char*,PropType::wr> Txt;
private:
	string m_txt;
};

TEST_CASE("Getters/Setters", "[gs]")
{
	Test a;
	a.count = 123; 				// set
	REQUIRE(a.count == 124);	// get

	a.pos = {1,2};
	println(">{}", static_cast<Position>(a.pos));
	Position p = a.pos;
	REQUIRE((p.x == 1 and p.y == 2));
}

TEST_CASE("Getters/Setters string", "[gs]")
{
	TestStr s;
	s.Txt = "Hello"; // ok
	// get will throw
	REQUIRE_THROWS_AS(string{s.Txt} == "Hello", PropException);
}
