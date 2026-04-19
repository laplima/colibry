// Bag Unit Testing
#include <print>
#include <set>
#include <colibry/Bag.h>
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

TEST_CASE("Bag constructors", "[constructors]")
{
	Bag<int> b{1,5};
	REQUIRE(b.lower() == 1);
	REQUIRE(b.upper() == 5);

	Bag<int> b2{3};
	REQUIRE(b2.lower() == 3);
}

TEST_CASE("Get tests", "[get]")
{
	Bag<int> b{1,5};

	SECTION("Linear get") {
		for (int i=0; i<5; ++i)
			REQUIRE(b.get() == i+1);
		REQUIRE(b.empty());
	}

	SECTION("Random get") {
		b.randomize();
		set<int> v;
		for (int i=0; i<5; ++i) {
			auto x = b.get();
			REQUIRE((x >= 1 && x <= 5));
			REQUIRE(!v.contains(x));
			v.insert(x);
		}
		REQUIRE(b.empty());
	}
}

TEST_CASE("Put Back tests", "[putback]")
{
	Bag<int> b{1,5};
	int x{};
	for (int i=0; i<5; ++i) {
		b >> x;
		if (i >=2)
			b << i-1;
	}
	REQUIRE(!b.empty());
	b >> x;	// remove 1
	REQUIRE(b.get() == 4);
}

TEST_CASE("Reset test", "[reset]")
{
	Bag<unsigned short> b;
	for (int i=0; i<5; ++i)
		b.get();
	b.reset();
	for (int i=0; i<5; ++i)
		REQUIRE(b.get() == i);
}

TEST_CASE("Size test", "[size]")
{
	Bag<unsigned short> b{1,5};
	REQUIRE(b.size() == 5);
	b.get();
	auto x = b.get();
	b.get();
	b.put_back(x);
	REQUIRE(b.size() == 3);
}
