//
// JSON-based Interactive Shell
// (based on linenoise.h)
//
// ----------------------- Define oserver class
//
// class MyCommands : public CmdObserver {
// public:
// 	MyCommands()
// 	{
// 		bind()
// 			("list", FWRAP(make_list));
// 	}

// 	void exit_(const Stringv& args) override {
// 		print("terminating\n");
// 		CmdObserver::exit_(args);
// 	}

// 	void make_list(const Stringv& args)
// 	{
// 		print("LIST\n>>{}\n", fmt::join(args.begin(), args.end(), ", "));
// 	}
// };
//
// ----------------------- Main
//
// MyCommands cmds;			// observer
// JIShell sh{cmds};		// subject
// PersistenceManager::load("cmds.json", sh);
// sh.set_prompt("=> ");
// sh.cmdloop();
//

#ifndef JISHELL_H
#define JISHELL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#define FWRAP(fn) [this](const std::vector<std::string>& a) { this->fn(a); }

namespace colibry {

	// TYPES
	using Stringv = std::vector<std::string>;
	using Method = std::function<void(const Stringv&)>;

	class CmdObserver;

	class EasyInit {
	public:
		explicit EasyInit(CmdObserver& owner) : owner{owner} {}
		EasyInit& operator()(const std::string& cmd, const Method& f);
	private:
		CmdObserver& owner;
	};

	// -------------------------------------------------------------------------
	// Observer handles the commands

	class CmdObserver {
	public:
		CmdObserver();
		void add(std::string cmd, std::string h);
		void stop() { running_ = false; }	// exit
		[[nodiscard]] bool running() const { return running_; }

		void bind(const std::string& cmd, const Method& f);
		EasyInit bind();

		friend class EasyInit;
		friend class JIShell;

	protected:

		// may be overloaded, though not typically
		virtual void dispatch(const Stringv& args);
		// default (overridable) commands (bound by default)
		virtual void help(const Stringv& args);
		virtual void exit_(const Stringv& args);

		struct CmdData {
			std::string description;
			Method f;
			CmdData() = default;
			explicit CmdData(std::string d) : description{std::move(d)} {}
		};
		std::map<std::string, CmdData> fmap;
		bool running_ = true;
	};

	// -------------------------------------------------------------------------
	// JIShell handles completion and calls dispatch on the observer

	class JIShell {
	public:

		explicit JIShell(CmdObserver& obs);
		void set_prompt(std::string p);
		void cmdloop();
		void add_cmd(const std::string& cmd, const std::string& desc, int ntokens);

		static void completion(std::string_view buffer, Stringv& compls, const std::vector<Stringv>& cmds);

	protected:

		std::string prompt;
		CmdObserver& cobs;		// doesn't own observer
		std::vector<Stringv> commands;
	};

	// -------------------------------------------------------------------------
	// Persistence Manager knows about file type to load the commands
	// specification from

	class PersistenceManager {
	public:
		static void load(const std::filesystem::path& cmdjson, JIShell& sh);
	private:
		struct CmdData {
			std::string cmd;
			std::string descr;
			bool operator<(const CmdData& c) const { return cmd < c.cmd; }
			bool operator==(const CmdData& c) const { return cmd == c.cmd; }
		};
		using Cmdv = std::vector<CmdData>;	// a comand and its completions
		using Commands = std::vector<Cmdv>;	// comands sorted by # of tokens
		static void build_command(const std::string& prefix,
			const nlohmann::json& cmdj,
			Commands& cmds, unsigned short ntok);
	};

	// helper functions

	std::vector<std::string> split(const std::string& input);
	inline auto count_tokens(const std::string& s) { return split(s).size(); }
	void trim(std::string& s);
	void trim(Stringv& sv);

} // namespace colibry

#endif
