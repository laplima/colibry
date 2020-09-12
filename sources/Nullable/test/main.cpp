#include <iostream>
#include "../Nullable.h"

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
    Nullable<int> i(2);
    Nullable<int> j(5);
    Nullable<int> k = 4;

    try {
	if (i.IsNull())
	    cout << *i << endl;
	else
	    cerr << "i: NULL" << endl;
	cout << j << endl;
	cout << *k << endl;
	if (j==k)
	    cout << "j is k" << endl;
	else
	    cout << "j is not k" << endl;
	
    } catch (IsNullExc&) {
	cerr << "NULL!" << endl;
    }
    
    return 0;
}
