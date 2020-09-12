#include <iostream>
#include <string>
#include "../Property.h"

using namespace std;
using namespace NCOLib;

struct Position {
	int x,y;
	Position(int a, int b) : x(a), y(b) {}
	Position() {}
};

ostream& operator<<(ostream& os, const Position& p)
{
	return (os << "(" << p.x << "," << p.y << ")");
}

class Test {
public:
	Test() : pos(this,&Test::getp,&Test::setp), count(this,&Test::get,&Test::set) {}

	int get() const { cout << "gcount()" << endl; return m_count; }
	void set(int x) { cout << "scount()" << endl; m_count = x; }

	Position getp() const { cout << "gpos()" << endl; return m_pos; }
	void setp(const Position p) { cout << "spos()" << endl; m_pos = p; }

	Property<Test,int,'a'> count;
	Property<Test,Position,'a'> pos;
private:
	int m_count;
	Position m_pos;
};

class TestStr {
public:
	TestStr()
	{
		Txt.setContainer(this);
		Txt.setter(&TestStr::setTxt);
		Txt.getter(&TestStr::getTxt);
	}

	void setTxt(const char* x) { m_txt = x; }
	const char* getTxt() const { return m_txt.c_str(); }

	Property<TestStr,const char*,'z'> Txt;
private:
	string m_txt;
};


int main(int argc, char* argv[])
{
	try {
		Test x,y;
		x.count = 123;
		int z = x.count + 1;
		cout << "z = " << z << endl;

		x.pos = Position(10,20);
		cout << x.pos << endl;

		TestStr a;
		a.Txt = "ola";
		cout << a.Txt << endl;
	} catch (PropException& e) {
		cerr << e << endl;
	}

	return 0;
}
