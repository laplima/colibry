//
// OLD!
//

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <signal.h>
#include "../Sockets.h"

using namespace std;
using namespace NCOLib;

#define ESCAPE (char)0x1B
#define LBRACKET (char)0x5B
#define FIRSTCOL ESCAPE << LBRACKET << 0 << 'D'
#define ONELINEUP ESCAPE << LBRACKET << 1 << 'A'
#define LINEUP ONELINEUP << FIRSTCOL

void terminate(int nsig);

int main(int argc, char* argv[])
{
    if (argc < 3) {
	cerr << "USAGE: " << argv[0] << " <host> <portn>" << endl;
	return 1;
    }

    signal(SIGINT,terminate);

    Socket sock;
    sock.Connect(argv[1],atoi(argv[2]));

    // while (1) {

    char buf[32];
    ssize_t sz = sock.Read(buf,sizeof(buf));
    buf[sz] = '\0';
    cout << "Time is: " << buf << endl;// << LINEUP;

    sleep(5);

    string msg ("HELLO");
    sock.Send(msg.c_str(),msg.size());

    sleep(5);
	//}

    sock.Close();
    return 0;
}

void terminate(int nsig)
{
    cout << "\nTerminating." << endl;
    exit(0);
}

