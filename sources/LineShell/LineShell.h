//
// LINE SHELL
// JSON-based Interactive Shell
// (based on linenoise.h)
// (C) 2022 by Luiz A. de P. Lima Jr.
// All rights reserved.
//
// See README.md file
//

#ifndef LINESHELL_H
#define LINESHELL_H

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

	class CmdObserver;

	namespace lineshell {

		// types
		using Stringv = std::vector<std::string>;
		using Method = std::function<void(const Stringv&)>;

		// helper functions
		Stringv splitargs(const std::string& input);
		Stringv::size_type count_tokens(const std::string& s);
		void trim(std::string& s);
		void trim(Stringv& sv);

		class EasyInit {
		public:
			explicit EasyInit(CmdObserver& owner) : owner{owner} {}
			EasyInit& operator()(const std::string& cmd, const lineshell::Method& f);
		private:
			CmdObserver& owner;
		};

	}

	// Observer handles the commands -------------------------------------------

	class CmdObserver {
	public:
		CmdObserver();
		void add(std::string cmd, std::string h);
		void stop() { running_ = false; }	// exit
		[[nodiscard]] bool running() const { return running_; }

		void bind(const std::string& cmd, const lineshell::Method& f);
		lineshell::EasyInit bind();

		friend class lineshell::EasyInit;
		friend class LineShell;

	protected:

		// may be overloaded, though not typically
		virtual void dispatch(const lineshell::Stringv& args);
		// default (overridable) commands (bound by default)
		virtual void help(const lineshell::Stringv& args);
		virtual void exit_(const lineshell::Stringv& args);

		struct CmdData {
			std::string description;
			lineshell::Method f;
			CmdData() = default;
			explicit CmdData(std::string d) : description{std::move(d)} {}
		};
		std::map<std::string, CmdData> fmap;
		bool running_ = true;
	};

	// LineShell ---------------------------------------------------------------
	// handles completion and calls dispatch on the observer

	class LineShell {
	public:

		explicit LineShell(CmdObserver& obs);
		void set_prompt(std::string p);
		void cmdloop();
		// manually add commands formed by ntokens+1 tokens
		void add_cmd(const std::string& cmd,
			const std::string& desc,
			int ntokens);	// number of tokens -1 in the cmd

		static void completion(std::string_view buffer,
			lineshell::Stringv& compls,
			const std::vector<lineshell::Stringv>& cmds);

	protected:

		std::string prompt;
		CmdObserver& cobs;		// doesn't own observer
		std::vector<lineshell::Stringv> commands;
	};

	namespace lineshell {

		// Persistence Manager -----------------------------------------------------
		// knows about file type to load the commands specification from

		class PersistenceManager {
		public:
			static void load_file(LineShell& sh, const std::filesystem::path& fpath);
			static void load_str(LineShell& sh, const std::string& s);
			static void load_json(LineShell& sh, const nlohmann::json& j);
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

	} // namespace lineshell

} // namespace colibry

#endif
