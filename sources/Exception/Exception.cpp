#include "Exception.h"

using namespace std;

ostream& operator<<(ostream& os, colibry::Exception& exc)
{
    return (os << exc.what() << " [" << exc.where() << "]");
}

