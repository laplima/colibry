#include "OptionManager.h"
#include <cstring>
// #include <iostream>

using namespace std;
using namespace colibry;

OptionManager::OptionManager(int argc, char* argv[])
{
	// parse and build map
	mandpar_ = nullptr;
	for (int i=1; i<argc; ++i) {
		if (argv[i][0] == '-') {
			// option found
			const char* arg = argv[i+1];
			if (arg != nullptr)
				if (*arg == '-')
					arg = nullptr;
			if (argv[i][1] == '-')
				optmap_[argv[i]+2] = arg;	// exclude "--"
			else
				optmap_[argv[i]+1] = arg;	// exclude '-'
			if (arg != nullptr)
				++i;
		} else {
			// mandatory parameter (just one allowed)
			// if multiple, just the last is considered
			mandpar_ = argv[i];
		}
	}

	// std::cout << "mandatory: " << (mandpar_ == nullptr ? "<empty>" : mandpar_) << std::endl;
	// for (auto& [option,par] : optmap_)
	// 	std::cout << option << ": " << (par==nullptr ? "" : par) << std::endl;
}

const char* OptionManager::param(const string& o)
{
	if (has(o))
		return optmap_[o];
	return nullptr;
}

bool OptionManager::has(const string& opt) const
{
	return (optmap_.count(opt)>0);
}
