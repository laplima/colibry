#include "OptionManager.h"
#include <cstring>
#include <iostream>

using namespace std;
using namespace colibry;

OptionManager::OptionManager(int argc, char* argv[]) : args_(argv,argc)
{
	auto getnext = [this](int& i) {
		if (i < this->args_.size() - 1) {
			if (this->args_[i+1][0] != '-') {
				++i;
				return string_view{this->args_[i]};
			}
		}
		return string_view{};
	};

	// parse and build map
	for (int i=1; i<args_.size(); ++i) {
		string_view arg{args_[i]};
		if (arg.starts_with("--"))
			optmap_[arg.substr(2).data()] = getnext(i);
		else if (arg.starts_with("-"))
			optmap_[arg.substr(1).data()] = getnext(i);
		else
			positional_.push_back(arg);
	}

	// std::cout << "lonely: " << (lonely_.empty() ? "<empty>" : lonely_) << std::endl;
	// for (auto& [option,par] : optmap_)
	// 	std::cout << option << ": " << (par.empty() ? "" : par) << std::endl;
}

string_view OptionManager::arg(const string& o) const
{
	if (o.empty())
		return positional_.empty() ? string_view{} : positional_[0];
	if (has(o))
		return optmap_.at(o);
	return string_view{};
}

std::string_view OptionManager::arg(const std::initializer_list<std::string>& opts) const
{
	for (const auto& o : opts)
		if (has(o))
			return optmap_.at(o);
	return string_view{};
}

const std::vector<std::string_view>& OptionManager::pargs() const
{
	return positional_;
}

bool OptionManager::has(const string& opt) const
{
	return (optmap_.contains(opt));
}

std::string_view OptionManager::program() const
{
	return args_[0];
}
