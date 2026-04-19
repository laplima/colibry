#include <EscapedText.h>
#include <TextStyle.h>
#include <print>
#include <colibry/TextTools.h>
#include <catch2/catch_test_macros.hpp>

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
    println("{}white!{}", colibry::set_color(255,255,255), reset_color());
    println("{}some color{}", set_color("#FF8000",true), reset_color());
    println("normal");

    println("{}", EscapedText{fg(color::brown) | emphasis::bold,
        "this is escaped"});

    println(bg(color::dark_cyan), "{} ({})", "oing boing"s, 123);
}

