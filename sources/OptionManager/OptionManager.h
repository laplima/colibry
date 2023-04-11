//
// (Basic) OptionManager
// (C) LAPLJ
//
// Build map:
//		option -> parameter (may be nullptr)
//
// option is stripped of initial '-' or '--'
//
// USAGE:
//
//	OptionManager opt{argc, argv};
//
//	string_view p1 = opt.arg("f");		// -f <p1>
//	string_view p2 = opt.arg("file");	// --file <p2>
//  auto p3 = opt.arg({"m","memory"});  // -m <p3> or --memory <p3>
//	if (opt.has("help")) {} 			// --help
//  string_view fname = opt.arg();		// first positional argument, e.g. "file.pdf"
//  auto vec = opt.pargs();				// vector of positional arguments

#ifndef OPTIONMANAGER_H
#define OPTIONMANAGER_H

#include <map>
#include <string>
#include <span>
#include <string_view>
#include <vector>
#include <initializer_list>

namespace colibry {

	class OptionManager {
	public:
		OptionManager(int argc, char* argv[]);
		[[nodiscard]] std::string_view arg(const std::string& opt = "") const;	// previously, param()
		[[nodiscard]] std::string_view arg(const std::initializer_list<std::string>& opts) const;
		// returns a vector of postional arguments,if any
		[[nodiscard]] const std::vector<std::string_view>& pargs() const;
		[[nodiscard]] bool has(const std::string& opt) const;    // parameter-less option
		[[nodiscard]] std::string_view program() const;
	private:
		std::span<char*> args_;
		std::map<std::string, std::string_view> optmap_; // option => argv[i]
		std::vector<std::string_view> positional_;		// positional arguments
	};

};

#endif
