//
// This is a simple implementation of a property
// (this seems to be better than the complex one)
//

#include <iostream>
#include "../Property2.h"

// -----------------------------------------------------------------------------

template <typename T>
class MyProp : public colibry::Property<T> {
public:
	MyProp(const T& v) : colibry::Property<T>{v} {}
protected:
	void setter(const T& v) {
		std::cout << "setter: " << v << std::endl;
		colibry::Property<T>::setter(v);
	}
	const T& getter() {
		std::cout << "getter" << std::endl;
		return colibry::Property<T>::getter();
	}
};

// -----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	using namespace std;

	MyProp<int> mp{123};
	cout << mp << endl;
	mp = 256;
	cout << mp << endl;
}
