// OLD
//
// clang++ timeserv.cpp ../Sockets.cpp -o timeserv
//

#include <iostream>
#include <string>
#include <time.h>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include "../Sockets.h"

using namespace std;
using namespace colibry;

void termination(int nsig);
void catch_sig(int nsig)
{
    cout << "SIGNAL = " << nsig << endl;
    exit(1);
}

inline int Max(const int a, const int b)
{
    return (a>b) ? a : b;
}

inline void Error(const string& msg)
{
    cerr << "Error: " << msg << endl;
    exit(1);
}

ServerSocket ssock(4002);

int main(int argc, char* argv[])
{
	for (int i=1; i<32; i++)
		signal (i,catch_sig);
	signal(SIGINT,termination);
	signal(SIGTERM,termination);

	cout << "Accepting connections at port # 4002..." << endl;

	fd_set master;
	fd_set readfs, writefs, exceptfs;

	FD_ZERO(&master);
	FD_ZERO(&readfs);
	FD_ZERO(&writefs);
	FD_ZERO(&exceptfs);

	int fdmax = ssock.fd();

	FD_SET(fdmax,&master);

	char buf[32];
	Socket sock;
	while (1) {

		cout << "Select" << endl;

		readfs = writefs = exceptfs = master;
	//memcpy(&readfs, &master, sizeof(master));
	//memcpy(&writefs, &master, sizeof(master));
	//memcpy(&exceptfs, &master, sizeof(master));
		if (select(fdmax+1,&readfs,NULL,&exceptfs,NULL) == -1)
			Error("Select");

	// SERVER SOCKET

		if (FD_ISSET(ssock.fd(),&readfs)) {
			cout << "SRV: In read set." << endl;
			sock = ssock.Accept();
			cout << "SRV: Accepted: " << sock.RemoteHost() << ":"
			<< sock.RemotePort() << endl;

			fdmax = Max(fdmax,sock.fd());
			FD_SET(sock.fd(),&master);

			time_t tm = time(NULL);
			string msg(ctime(&tm));
			msg.erase(msg.size()-1);

			sock.Send(msg.c_str(),msg.size());
			cout << "SENT" << endl;
		}

	/* if (FD_ISSET(ssock.fd(),&writefs)) {
	    cout << "In write set." << endl;
	    } */

		if (FD_ISSET(ssock.fd(),&exceptfs)) {
			cout << "In exception set." << endl;
		}

	// CLIENT SOCKET

		if (sock.IsConnected()) {
			if (FD_ISSET(sock.fd(),&readfs)) {
				cout << "CLNT: in read set." << endl;
				ssize_t sz = sock.Receive(buf,sizeof(buf));
				if (sz == 0) {
		    // disconnected
					cout << "Closed connection." << endl;
					FD_CLR(sock.fd(),&master);
					sock.Close();
				} else {
					buf[sz] = '\0';
					cout << "Received: " << buf << endl;
				}

		//string msg("HELLO");
		//sock.Send(msg.c_str(),msg.size());
			}
	    /*if (FD_ISSET(sock.fd(),&writefs)) {
		cout << "CLNT: in write set." << endl;
		string msg("HELLO");
		sock.Send(msg.c_str(),msg.size());
	    }*/
			if (FD_ISSET(sock.fd(),&exceptfs)) {
				cout << "CLNT: in exception set." << endl;
			}
		}

	}

	return 0;
}

void termination(int nsig)
{
	cout << "\nQuitting." << endl;
	ssock.Close();
	exit(0);
}
