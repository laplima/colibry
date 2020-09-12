#include <iostream>
#include <string>
#include <time.h>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include "../Sockets.h"

using namespace std;
using namespace NCOLib;

void termination(int nsig);
void catch_sig(int nsig)
{
    cout << "SIGNAL = " << nsig << endl;
    exit(1);
}

inline void Error(const string& msg)
{
    cerr << "Error: " << msg << endl;
    exit(1);
}

#define PORT 4002

int main(int argc, char* argv[])
{
    for (int i=1; i<32; i++)
	signal (i,catch_sig);
    signal(SIGINT,termination);
    
    char buf[32];
    SocketSelector socksel;
    socksel.Add(new ServerSocket(PORT));
    SockVec sockvec;
    Socket* sock;

    cout << "Accepting connections at port " << PORT << "..." << endl;

    while (1) {

	cout << "Select" << endl;
	socksel.ReadSelect(&sockvec);

	for (int i=0; i<sockvec.size(); i++) {
	    if (sockvec[i]->type() == SERVER_SOCK) {
		// accept
		ServerSocket* ss = static_cast<ServerSocket*>(sockvec[i]);
		socksel.Add(sock=ss->Accept());
		cout << "SRV: Accepted: " << sock->RemoteHost() << ":"
		     << sock->RemotePort() << endl;

		time_t tm = time(NULL);
		string msg(ctime(&tm));
		msg.erase(msg.size()-1);

		sock->Send(msg.c_str(),msg.size());
		cout << "SENT" << endl;

	    } else if (sockvec[i]->type() == CLIENT_SOCK) {
		sock = static_cast<Socket*>(sockvec[i]);
		cout << "CLNT: in read set." << endl;
		ssize_t sz = sock->Receive(buf,sizeof(buf));
		if (sz == 0) {
		    // disconnected
		    cout << "Closed connection." << endl;
		    socksel.Remove(sock);
		} else {
		    buf[sz] = '\0';
		    cout << "Received: " << buf << endl;
		}
	    }
	}

    }

    return 0;
}

void termination(int nsig)
{
    cout << "\nQuitting." << endl;
    exit(0);
}
