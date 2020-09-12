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
bool NC_CLIENT = false;

int main(int argc, char* argv[])
{
	ServerSocket tsock{DEFAULT_PORT};

	cout << "Echo server listening on port " << DEFAULT_PORT << endl;

	unsigned short workerno = 0;
	while (true) {

		cout << "S: acccepting connections..." << endl;
		unique_ptr<Socket> csock{tsock.Accept()};
		++workerno;

		pid_t p = fork();
		if (p == 0) {
			tsock.Close();
			string msg;
			try {
				*csock >> msg;
				while (!msg.empty()) {
					if (NC_CLIENT)
						msg = msg.substr(0,msg.size()-1);	// remove newline
					cout << "\t" << workerno << ": Got \"" << msg << "\"" << endl;
					msg += "_ECHO";
					*csock << msg;
					if (NC_CLIENT)
						*csock << "\n";
					// get next message
					*csock >> msg;
				}
			} catch (Socket::Disconnected& e) {
				cout << "\t" << workerno << ": Client disconnected!" << endl;
			}
			return 0;
		}
		csock->Close();
	}
}
