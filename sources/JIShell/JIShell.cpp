#include "JIShell.h"
#include <functional>
#include <stdexcept>
#include <vector>
#include <regex>
#include "linenoise.h"

using namespace std;
using namespace colibry;
using json = nlohmann::json;
using std::runtime_error;

// ----------------------------------------------------------------------------

EasyInit& EasyInit::operator()(const string& cmd, const Method& f)
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

void CmdObserver::bind(const std::string& cmd, const Method& f)
{
	fmap[cmd].f = f;
}

EasyInit CmdObserver::bind()
{
	return EasyInit{*this};
}

void CmdObserver::dispatch(const Stringv& args)
{
	if (fmap.count(args[0]) == 0)
		throw runtime_error{fmt::format("unknown command: {}", args[0])};
	fmap.at(args[0]).f({args.begin()+1, args.end()});
}

void CmdObserver::help(const Stringv& /*unused*/)
{
	for (const auto& [cmd, cdata] : fmap)
		fmt::print("{:>10} :  {}\n", cmd, cdata.description);
}

void CmdObserver::exit_(const Stringv& /*unused*/)
{
	stop();
}

// ----------------------------------------------------------------------------

JIShell::JIShell(CmdObserver& obs) : cobs{obs}
{
	auto cc = [this](const char* eb, vector<string>& c) {
		JIShell::completion(eb, c, this->commands);
	};

	linenoise::SetCompletionCallback(cc);
	linenoise::SetMultiLine(false);
	linenoise::SetHistoryMaxLen(4);
	linenoise::LoadHistory("history.txt");
}

void JIShell::completion(std::string_view buffer, Stringv& compls,
	const std::vector<Stringv>& cmds)
{
	string b{string{buffer}};
	trim(b);
	auto tokens = count_tokens(b);

	if (tokens > cmds.size() || tokens == 0)
		return;

	const Stringv& sv = cmds[tokens-1];
	if (std::find(sv.begin(), sv.end(), b) != sv.end())
		++tokens;

	for (const auto&c : cmds[tokens-1])
		if (c.starts_with(buffer))
			compls.push_back(c);
}

void JIShell::set_prompt(std::string p)
{
	prompt = std::move(p);
}

void JIShell::add_cmd(const string& cmd, const string& desc, int ntok)
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

void JIShell::cmdloop()
{
	fmt::print("This is the command loop!\n");
	std::string cmd;
	while (cobs.running()) {
		linenoise::Readline(prompt.c_str(), cmd);
		trim(cmd);
		auto cmdline = split(cmd);
		trim(cmdline);
		try {
			cobs.dispatch(cmdline);
			linenoise::AddHistory(cmd.c_str());
		} catch(const runtime_error& e) {
			fmt::print(stderr, "{}\n", e.what());
		} catch (const bad_function_call&) {
			fmt::print(stderr, "no function bound to command \"{}\"\n", cmdline[0]);
		}
	}
	linenoise::SaveHistory("history.txt");
}

// ----------------------------------------------------------------------------

void PersistenceManager::load(const std::filesystem::path& cmdjson,
	JIShell& sh)
{
	std::ifstream input{cmdjson};
	if (!input)
		throw std::runtime_error{fmt::format(R"(file "{}" not found)",
			cmdjson.string())};
	json ctree;		// command tree
	input >> ctree;

	Commands cmds;
	for (const auto& cmd : ctree)
		build_command("", cmd, cmds, 0);

	for (int i=0; i<cmds.size(); ++i)
		for (const auto& cd : cmds[i]) {
			sh.add_cmd(cd.cmd,cd.descr,i);
			// fmt::print(">> {} ({})\n", cd.cmd, cd.descr);
		}
}

void PersistenceManager::build_command(const std::string& prefix,
	const json& cmdj, Commands& cmds, unsigned short ntok)
{
	const auto& first = cmdj.items().begin();
	std::string cmdstr = (prefix.empty() ? first.key() : fmt::format("{} {}", prefix, first.key()));
	std::string helpstr = first.value()["desc"].get<std::string>();
	try {
		cmds.at(ntok).push_back({ cmdstr, helpstr });
	} catch (const std::out_of_range& ) {
		cmds.push_back({{ cmdstr, helpstr }});
	}
	for (const auto& nextcmd : first.value().at("args"))
		build_command(cmdstr, nextcmd, cmds, ntok+1);
}

// ----------------------------------------------------------------------------

std::vector<std::string> colibry::split(const std::string& input)
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

void colibry::trim(std::string& s)
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

void colibry::trim(Stringv& sv)
{
	for (auto& s : sv)
		trim(s);
}
