#include "LineShell.h"
#include <functional>
#include <stdexcept>
#include <vector>
#include <regex>
#include <algorithm>
#include "linenoise.h"

using namespace colibry;
using json = nlohmann::json;
using std::string;
using std::runtime_error;

// ----------------------------------------------------------------------------

lineshell::EasyInit& lineshell::EasyInit::operator()(
	const string& cmd,
	const Method& f)
{
	owner.bind(cmd, f);
	return *this;
}

// ----------------------------------------------------------------------------

CmdObserver::CmdObserver()
{
	bind()
		("help", FWRAP(help))
		("exit", FWRAP(exit_));
}

void CmdObserver::add(std::string cmd, std::string h)
{
	fmap[std::move(cmd)].description = std::move(h);
}

void CmdObserver::bind(const std::string& cmd, const lineshell::Method& f)
{
	fmap[cmd].f = f;
}

lineshell::EasyInit CmdObserver::bind()
{
	return lineshell::EasyInit{*this};
}

void CmdObserver::dispatch(const lineshell::Stringv& args)
{
	if (!fmap.contains(args[0]))
		throw runtime_error{fmt::format("unknown command: {}", args[0])};
	// fmap.at(args[0]).f({args.begin()+1, args.end()});
	fmap.at(args[0]).f(args);
}

void CmdObserver::help(const lineshell::Stringv& /*unused*/)
{
	for (const auto& [cmd, cdata] : fmap)
		fmt::print("{:>10} :  {}\n", cmd, cdata.description);
}

void CmdObserver::exit_(const lineshell::Stringv& /*unused*/)
{
	stop();
}

// ----------------------------------------------------------------------------

LineShell::LineShell(CmdObserver& obs) : cobs{obs}
{
	auto cc = [this](const char* eb, std::vector<string>& c) {
		LineShell::completion(eb, c, this->commands);
	};

	using namespace linenoise;

	SetCompletionCallback(cc);
	SetMultiLine(false);
	SetHistoryMaxLen(4);
	LoadHistory("history.txt");
}

void LineShell::completion(std::string_view buffer,
	lineshell::Stringv& compls,
	const std::vector<lineshell::Stringv>& cmds)
{
	using namespace lineshell;

	string b{string{buffer}};
	trim(b);
	auto tokens = count_tokens(b);

	if (tokens > cmds.size() || tokens == 0)
		return;

	const Stringv& sv = cmds[tokens-1];
	if (std::find(sv.begin(), sv.end(), b) != sv.end())
		++tokens;
	if (tokens > cmds.size())
		return;

	for (const auto&c : cmds[tokens-1])
		if (c.starts_with(buffer))
			compls.push_back(c);
}

void LineShell::set_prompt(std::string p)
{
	prompt = std::move(p);
}

void LineShell::add_cmd(const string& cmd, const string& desc, int ntok)
{
	if (ntok >= commands.size())
		commands.resize(ntok+1);
	commands[ntok].push_back(cmd);
	// sort
	std::sort(std::begin(commands[ntok]), std::end(commands[ntok]));
	// update observer
	if (ntok == 0)
		cobs.add(cmd, desc);
}

void LineShell::rm_cmd(const std::string& cmd, int ntok)
{
	auto p = std::find(std::begin(commands[ntok]), std::end(commands[ntok]), cmd);
	if (p != std::end(commands[ntok]))
		commands[ntok].erase(p);
}

void LineShell::cmdloop()
{
	using namespace lineshell;
	string cmd;
	while (cobs.running()) {
		linenoise::Readline(prompt.c_str(), cmd);
		trim(cmd);
		auto cmdline = splitargs(cmd);
		trim(cmdline);
		if (cmdline.empty())
			continue;
		try {
			cobs.dispatch(cmdline);
			linenoise::AddHistory(cmd.c_str());
		} catch(const std::runtime_error& e) {
			fmt::print(stderr, "{}\n", e.what());
		} catch (const std::bad_function_call&) {
			fmt::print(stderr, "no function bound to command \"{}\"\n", cmdline[0]);
		}
	}
	linenoise::SaveHistory("history.txt");
}

// ----------------------------------------------------------------------------

void lineshell::PersistenceManager::load_file(
	LineShell& sh,
	const std::filesystem::path& fpath)
{
	std::ifstream input{fpath};
	if (!input)
		throw std::runtime_error{fmt::format(R"(file "{}" not found)",
			fpath.string())};
	json ctree;		// command tree
	input >> ctree;
	load_json(sh, ctree);
}

void lineshell::PersistenceManager::load_str(
	LineShell& sh,
	const std::string& s)
{
	json ctree = json::parse(s);
	load_json(sh, ctree);
}

void lineshell::PersistenceManager::load_json(
	LineShell& sh,
	const nlohmann::json& j)
{
	Commands cmds;
	for (const auto& cmd : j)
		build_command("", cmd, cmds, 0);

	for (int i=0; i<cmds.size(); ++i)
		for (const auto& cd : cmds[i])
			sh.add_cmd(cd.cmd,cd.descr,i);
}

void lineshell::PersistenceManager::build_command(
	const std::string& prefix,
	const json& cmdj,
	Commands& cmds,
	unsigned short ntok)
{
	const auto& first = cmdj.items().begin();
	string cmdstr = (prefix.empty() ?
		first.key() : fmt::format("{} {}", prefix, first.key()));
	string helpstr = first.value()["desc"].get<string>();
	try {
		cmds.at(ntok).push_back({ cmdstr, helpstr });
	} catch (const std::out_of_range& ) {
		cmds.push_back({{ cmdstr, helpstr }});
	}
	for (const auto& nextcmd : first.value().at("args"))
		build_command(cmdstr, nextcmd, cmds, ntok+1);
}

// ----------------------------------------------------------------------------

lineshell::Stringv lineshell::splitargs(const string& input)
{
	// split by spaces (text under quotes is not split by spaces)
	std::regex re{R"([^\s"']+|"[^"]*"|'[^']*')"};
	auto first = std::sregex_iterator(std::begin(input), std::end(input), re);
	auto last = std::sregex_iterator();
	std::vector<std::string> vs;
	for (auto i = first; i != last; ++i)
		vs.emplace_back(i->str());
	return vs;
}

lineshell::Stringv::size_type lineshell::count_tokens(const string& s)
{
	return splitargs(s).size();
}

void lineshell::trim(string& s)
{
	const char* pattern = " \t\n\r\"'";
	if (s.empty())
		return;
	auto inf = s.find_first_not_of(pattern);
	auto sup = s.find_last_not_of(pattern);
	if (inf == string::npos || sup == string::npos)
		s.clear();
	else
		s = s.substr(inf, sup-inf+1);
}

void lineshell::trim(Stringv& sv)
{
	for (auto& s : sv)
		trim(s);
}
