#include <iostream>
#include <string>
#include "../Dictionary.h"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		colibry::Dictionary st;
		string symbol;

		cout << "[" << st.Size() << "] Symbol: ";
		cin >> symbol;
		while (symbol != "exit") {
			auto i = st.LookUp(symbol);
			cout << "\t" << symbol << "\t--> " << i << endl;
			cout << "[" << st.Size() << "] Symbol: ";
			cin >> symbol;
		}

		cout << "\"ola\" = " << st["ola"] << endl;
		cout << "1 = \"" << st[1] << "\"" << endl;

		st.Remove(1);
		cout << "1 = " << st[1] << endl;	// may cause a problem
	} catch(const exception& e) {
		cerr << e.what() << endl;
	}
}
