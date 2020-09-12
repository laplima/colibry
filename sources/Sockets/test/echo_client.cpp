#include <iostream>
#include <string>
#include <memory>
#include <signal.h>
#include <ctime>
#include <colibry/Sockets.h>
#include <unistd.h>

using namespace std;
using namespace colibry;

const unsigned short DEFAULT_PORT = 1512;

int main(int argc, char* argv[])
{
	try {
		Socket csock;
		csock.Connect("localhost",DEFAULT_PORT);

		string msg, resp;
		while (true) {
			cout << "Message: ";
			getline(cin,msg);
			csock << msg;
			if (msg == "exit") break;
			csock >> resp;
			cout << "\tResponse = \"" << resp << "\"" << endl;
		}
		csock.Close();
	} catch (const exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
