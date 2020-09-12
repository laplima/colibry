#include <iostream>
#include <unistd.h>
#include "../Thread.h"

using namespace std;
using namespace colibry;

class Counter : public Thread {
public:
	Counter(const char* name) : Thread(name) {}
	void SetTime(const float t) { tm = t; }
protected:
	void Run();
private:
	float tm;
};

void Counter::Run()
{
	for (int i=0; i<30; i++) {
		cout << Name() << ": " << i << endl;
		usleep((unsigned long)(1000000.0 * tm));
	}
}

int main(int argc, char* argv[])
{
	Counter c1("c1"), c2("c2");

	c1.SetTime(0.5F);
	c2.SetTime(0.3F);

	c1.Start();
	sleep(5);
	c2.Start();

	c1.Wait();
	c2.Wait();

	return 0;
}
