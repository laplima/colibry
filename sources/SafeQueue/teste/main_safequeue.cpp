#include <iostream>
#include "../SafeQueue.h"

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
    SafeQueue<int,10> sq;

    sq.insert(1);
    sq.insert(2);

    int x;
    while (sq.try_remove(x))
	   cout << x << endl;
    
    return 0;
}
