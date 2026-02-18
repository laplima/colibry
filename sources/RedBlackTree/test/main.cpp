#include <print>
#include <vector>
#include "../RBTree.h"

using namespace std;
using namespace colibry;

void f(int x)
{
	println("\t{} ", x);
}

int main(int argc, char* argv[])
{
	println("Red-Black Binary Search Trees");

	vector<int> v{20,10,30,5,40,35,32,34,33};
	RBTree<int> rbt;

	print("Inserting:\n\t");
	for (int i : v) {
        print("{} ", i);
		rbt.Insert(i);
	}
    println();

	print("Traversing (in-order):\n");
	rbt.Traverse(f);
    println("\n");

	println("Min = {}", rbt.Minimum()->key);
	println("Max = {}\n", rbt.Maximum()->key);

	auto nd = rbt.Search(32);
	if (nd != rbt.nil()) {
		auto sc = rbt.Successor(nd);
		println("Successor (32) = {}", sc->key);
		sc = rbt.Predecessor(nd);
		println("Predecessor (32) = {}", sc->key);
	}

	println("Levels:");
	for (int i : v)
		println("\t{}\t{}", i, rbt.Level(rbt.Search(i)));
}

