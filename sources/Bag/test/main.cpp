#include <iostream>
#include <fstream>
#include "../Bag.h"

using namespace std;
using namespace colibry;

int main(int argc, char* argv[])
{
	try {

		cout << "BAG3" << endl;
		Bag<int> b1{1,5};
		cout << b1 << endl;
		b1.randomize(true);

		for (int i=0; i<b1.upper(); ++i) {
			cout << "got: " << b1.get() << endl;
			cout << b1 << endl;
		}

		Bag<int> b2{0};
		cout << b2 << endl;
		int x;
		for (int i=0; i<10; ++i) {
			b2 >> x;
			cout <<  "got: " << x << endl;
			cout << b2 << endl;
		}

		b1.put_back(1);
		cout << b1 << endl;
		b1.put_back(5);
		cout << b1 << endl;
		b1.put_back(3);
		cout << b1 << endl;

		cout << "Save" << endl;
		ofstream out{"out.bag"};
		out << b1;
		out.close();

		b1.put_back(2);
		cout << b1 << endl;
		b1.put_back(4);
		cout << b1 << endl;
		// b1.put_back(20);

		cout << "Restore" << endl;
		ifstream in{"out.bag"};
		in >> b1;
		in.close();

		cout << b1 << endl;

	} catch (const runtime_error& e) {
		cerr << "runtime error: " << e.what() << endl;
	} catch (const logic_error& e) {
		cerr << "logic error: " << e.what() << endl;
	}
}
