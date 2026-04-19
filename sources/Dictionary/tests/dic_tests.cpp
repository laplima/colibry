#include <string>
#include <colibry/Dictionary.h>
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

TEST_CASE("Basic usage", "[dict]")
{
	Dictionary st;
	INFO("size = " << st.size() << '\n');
	REQUIRE(st.size() == 0);

	auto symbols = { "hello"s, "you"s, "are"s, "loved"s };
	for (const auto& s : symbols) {
		auto i = st.lookup(s);
		REQUIRE(st.size() == i+1);
		INFO(s << "\t--> " << i << "   [" << st.size() << "]\n");
	}

	SECTION("Basic methods") {
		REQUIRE(st["are"] == 2);
		REQUIRE(st[1] == "you");

		INFO("Removing");
		st.remove(1);
		REQUIRE(st.size() == symbols.size()-1);

		INFO("Accessing invalid elements");
		REQUIRE_THROWS(st[1]);

		REQUIRE(st.lookup("(yes)") == 1);
	}

	SECTION("Copying") {
		INFO("Copying dictionaries");
		colibry::Dictionary d{st};	// copy
		REQUIRE(d.size() == st.size());

		colibry::Dictionary d2{std::move(d)};	// move
		REQUIRE(d2.size() == st.size());
		REQUIRE(d.size() == 0);
	}
}
