#include <string>
#include "../PSemaphore.h"
#include "../SharedMemory.h"
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

TEST_CASE("Basic usage", "[ipc]")
{
	SECTION("Semaphore creation") {

		INFO("Will semaphore be created?");
		REQUIRE_THROWS(Semaphore{"s1"});

		INFO("Is s2 persistent?");
		Semaphore s2{"s2", 0, true};	// persist
		CHECK(s2.created());
		REQUIRE(s2.persistent());
		REQUIRE_FALSE(s2.try_down());

		Semaphore s3{"s3", 1};		// initial value
		CHECK(s3.created());
		REQUIRE_FALSE(s3.persistent());
		REQUIRE(s3.try_down());
	}
}
