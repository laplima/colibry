#include <iostream>
#include <string>
#include <cstdlib>
#include "../Bag.h"

using namespace std;
using namespace NCOLib;

int main(int argc, char* argv[])
{
    typedef unsigned short ID;
    
    Bag<ID> bag;

    bag.Randomize();

    ID i;
    string input;
    cout << "ID: ";
    getline(cin,input);
    while (input!="x") {

	try {
	    if (input.empty()) {
		i = bag.Get();
		cout << "Get() = " << i << endl;
	    } else {
		i = atoi(input.c_str());
		bag.PutBack(i);
	    }
	} catch (Bag<ID>::EmptyBagException&) {
	    cerr << "EXC: Empty bag" << endl;
	} catch (Bag<ID>::AlreadyInBagException&) {
	    cerr << "EXC: Already in bag" << endl;
	} catch (Bag<ID>::OutOfBoundsException&) {
	    cerr << "EXC: Out of bounds" << endl;
	}

	bag.Show();
	
	cout << "ID: ";
	getline(cin,input);
    }
    
    return 0;
}
