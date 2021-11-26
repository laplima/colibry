#include <iostream>
#include "../Logger.h"

// g++ ../Logger.cpp -o tlog -L../../../lib -I ../../../include -lcolibry main.cpp

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
    SET_REPORTING_LEVEL(DEBUG0);
    LOG(ERROR) << "Erro!";
    LOG(INFO) << "Info " << 123;
    
    return 0;
}
