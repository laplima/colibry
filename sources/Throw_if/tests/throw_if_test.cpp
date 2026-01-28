#include <iostream>
#include <string>
#include <stdexcept>
#include "../throw_if.h"
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

class MyException {
public:
	MyException(const string& w) : what_{w} {}
	string what() const { return what_; }
private:
	string what_;
};

class E2 : public runtime_error {
public:
	E2(const string& w) : runtime_error{w} {}
};

TEST_CASE("throwif testings", "[tif]")
{
	REQUIRE_NOTHROW(throwif(false, "This won't throw"));
	REQUIRE_THROWS_AS(throwif(true, "This will throw"), std::runtime_error);
	REQUIRE_THROWS_AS(throwif<E2>(true,"problem"), E2);
	REQUIRE_THROWS_AS(require<logic_error>(false,"assert failed"), logic_error);
}
