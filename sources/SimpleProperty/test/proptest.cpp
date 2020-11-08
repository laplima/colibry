//
// This is a simple implementation of a property
// (this seems to be better than the complex one)
//

#include <iostream>
#include "../SimpleProperty.h"

// -----------------------------------------------------------------------------

template <typename T>
class MyProp : public colibry::SimpleProperty<T> {
public:
	MyProp(const T& v) : colibry::SimpleProperty<T>{v} {}
protected:
	void setter(const T& v) {
		std::cout << "setter: " << v << std::endl;
		colibry::SimpleProperty<T>::setter(v);
	}
	const T& getter() {
		std::cout << "getter" << std::endl;
		return colibry::SimpleProperty<T>::getter();
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
