#include <iostream>
#include <vector>
#include "../RBTree.h"

using namespace std;
using namespace colibry;

void f(int x)
{
	cout << x << " ";
}

int main(int argc, char* argv[])
{
	cout << "Red-Black Binary Search Trees\n" << endl;

	vector<int> v{20,10,30,5,40,35,32,34,33};
	RBTree<int> rbt;

	cout << "Inserting:\n\t";
	for (int i : v) {
		cout << i << " ";
		rbt.Insert(i);
	}
	cout << endl;

	cout << "Traversing (in-order):\n\t";
	rbt.Traverse(f);
	cout << endl << endl;

	cout << "Min = " << rbt.Minimum()->key << endl;
	cout << "Max = " << rbt.Maximum()->key << endl << endl;

	auto nd = rbt.Search(32);
	if (nd != rbt.nil()) {
		auto sc = rbt.Successor(nd);
		cout << "Successor (32) = " << sc->key << endl;
		sc = rbt.Predecessor(nd);
		cout << "Predecessor (32) = " << sc->key << endl;
	}

	cout << "Levels:\n";
	for (int i : v)
		cout << "\t" << i << "\t" << rbt.Level(rbt.Search(i)) << endl;
}
