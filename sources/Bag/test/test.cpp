#include <print>
#include <format>
#include "../Bag.h"
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

TEST_CASE("Bag 1-5")
{
	Bag<int> b{1,5};
	auto x = b.get();
	REQUIRE(x == 1);
	REQUIRE(b.upper() == 5);
	for (int i=0; i<4; ++i) {
		x = b.get();
		REQUIRE(x == i+2);
	}
	REQUIRE(b.empty());
}

TEST_CASE("Bag 5", "[bag5]")
{
	Bag<int> b{5};
	int x{};
	b >> x;
	REQUIRE(x == 5);
	REQUIRE(b.lower() == 5);
	b >> x;
	b.put_back(5);
	x = b.get();
	x = b.get();
	REQUIRE(x == 7);

	b.reset();
	REQUIRE(b.get() == 5);
}
