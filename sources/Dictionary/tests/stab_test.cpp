#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include "../Dictionary.h"

using namespace std;

int main(int argc, char* argv[])
{
	try {
		colibry::Dictionary st;
		auto symbols = { "hello"s, "you"s, "are"s, "loved"s };

		cout << "Basic Usage\n";
		cout << "size = " << st.size() << '\n';

		for (const auto& s : symbols) {
			auto i = st.lookup(s);
			cout << s << "\t--> " << i << "   [" << st.size() << "]\n";
		}

		cout << "\n\"are\" -> " << st["are"] << '\n';
		cout << "1 -> " << st[1] << '\n';

		st.remove(1);
		cout << "index 1 removed\n";

		try {
			cout << "accessing index 1\n";
			cout << "you: " << st[1] << endl;
		} catch (const std::exception& e) {
			cerr << "ERROR: " << e.what() << endl;
		}

		auto i = st.lookup("(yes)");
		cout << "(yes)\t--> " << i << '\n';

		cout << "\nCopy Handling\n";

		colibry::Dictionary d{st};	// copy
		cout << "d size = " << d.size() << endl;

		colibry::Dictionary d2{std::move(d)};	// move
		cout << "d2 size = " << d2.size() << endl;
		cout << "d size = " << d.size() << endl;

	} catch(const exception& e) {
		cerr << e.what() << endl;
	}
}
