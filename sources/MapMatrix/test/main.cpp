#include <iostream>
#include <string>
#include "../MapMatrix.h"

using namespace std;
using namespace NCOLib;

int main(int argc, char* argv[])
{
    MapMatrix<int,bool> m;
    m(10,5) = false;
    m(1,1) = true;

    cout << m(10,5) << endl;
    cout << m(1,1) << endl;
    cout << m(3,3) << endl;
    
    return 0;
}
