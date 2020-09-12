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
const string DEFAULT_MC_ADDR = "239.0.0.2";
bool NC_CLIENT = false;

int main(int argc, char* argv[])
{
	UServerSocket usock{DEFAULT_MC_ADDR, DEFAULT_PORT};

	cout << "Echo server (MULTICAST) listening on:\n\t"
		<< DEFAULT_MC_ADDR << ":" << DEFAULT_PORT << endl;

	try {
		while (true) {

			cout << "S: accepting datagrams..." << endl;

			char buf[1024];
			auto bytes = usock.Receive(buf,1024);
			buf[bytes] = '\0';
			cout << "\t\"" << buf << "\" received from " <<
				usock.getOriginIP() << ":" << usock.getOriginPort() << endl;
		}
	} catch (...) {
		cout << "\tCommunication failure!" << endl;
	}
}
