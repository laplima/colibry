//
// (C) 2018 by laplima
// Check out this site:
//
// https://www.geeksforgeeks.org/little-and-big-endian-mystery/
//

#include <iostream>
#include <iomanip>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char* argv[])
{
	const unsigned short SZ = sizeof(unsigned short);
	union Word {
		unsigned short s;
		unsigned char c[SZ];
	};

	Word w;
	w.s = 0x0102;
	cout << "Host (local): ";
	if (SZ == 2) {
		if (w.c[0] == 1 && w.c[1] == 2)
			cout << "big endian" << endl;
		else if (w.c[0] == 2 && w.c[1] == 1)
			cout << "little endian" << endl;
		else
			cout << "unknown" << endl;
	} else
		cout << "sizeof(unsigned short) = " << SZ << endl;

	cout << "Sample conversion:" << endl;
	cout << "\t        host (local) = 0x0" << hex << w.s << endl;
	cout << "\tnetwork (big endian) = 0x0" << hex << htons(w.s) << endl;
}
