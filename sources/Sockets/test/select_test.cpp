#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define STDIN 0  // file descriptor for standard input

int main(void)
{
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 2;
    tv.tv_usec = 500000;

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);

    // don't care about writefds and exceptfds:
    select(STDIN+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(STDIN, &readfds))
        cout << "A key was pressed!\n" << endl;
    else
        cout << "Timed out.\n" << endl;

    return 0;
} 
