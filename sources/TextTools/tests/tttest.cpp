#include <iostream>
#include "../TextTools.h"

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
    cout << colibry::set_color(255,255,255) << "white!" << colibry::reset_color() << endl;
    cout << set_color("#FF8000",true) << "some color" << reset_color() << endl;
    cout << "normal" << endl;
}

