#include <iostream>
#include <string>
#include <memory>
#include <signal.h>
#include <chrono>
#include <ctime>
#include <colibry/Sockets.h>

using namespace std;
using namespace colibry;
using namespace chrono;

int main(int argc, char* argv[])
{
	ServerSocket tsock{4002};

	while (true) {
		unique_ptr<Socket> csock{tsock.Accept()};

		auto today = system_clock::now();
		time_t tt = system_clock::to_time_t(today);

		*csock << ctime(&tt);
	}
}
