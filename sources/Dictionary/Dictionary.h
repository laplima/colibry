//-----------------------------------------------------------------------------
//
// DICTIONARY CLASS INTERFACE DEFINITION
//
//     Common Object Library (colibry)
//
//     Version: April 2017-20
//
//     Description: Instead of dealing with string comparison, assignment,
//                  etc. it is easier (and memory saving) to deal with
//                  indexes that uniquely represent these strings.
//                  Conversion from indexes into strings (symbols) and
//                  vice-versa are available whenever needed.
//					Useful if there are many string replicas and for
//					greater-than-4-character strings.
//
//     Copyright (C) 1996-2017 by LAPLJ. All rights reserved.
//
//     OS: UNIX / WINDOWS / MACOS
//
//-----------------------------------------------------------------------------

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <cinttypes>
#include <Bag.h>

// forward declaration of friend function

namespace colibry { class Dictionary; }
void swap(colibry::Dictionary&, colibry::Dictionary&) noexcept;

// Interface

namespace colibry {

	// --------------------
	// --- Dictionary Class
	// --------------------

	class Dictionary {
	public:

		Dictionary();
		Dictionary(const Dictionary& d) = default;
		Dictionary(Dictionary&& d) noexcept;
		virtual ~Dictionary();
		Dictionary& operator=(Dictionary d) noexcept;

		friend void ::swap(Dictionary& d1, Dictionary& d2) noexcept;
		void swap(Dictionary& d) noexcept;

		// accessors (symbol/index must exist)
		virtual uint64_t operator[](const std::string& symbol);			// throw if not found
		virtual const std::string& operator[](const uint64_t index);	// idem

		// find/insert
		virtual uint64_t lookup(const std::string &symbol);

		virtual void remove(const uint64_t index);
		virtual void remove(const std::string &symbol);
		virtual void clear() noexcept;

		// Mark/un-mark existing symbol. Throw exception, if index is invalid
		virtual void mark(const uint64_t index, bool marked=true);

		// Check whether symbol is marked.
		[[nodiscard]] virtual bool is_marked(const uint64_t index) const;

		// Get table size
		[[nodiscard]] auto size() const { return maps2i_.size(); }

	private:

		inline void check_index(uint64_t index) const;

		struct Item {
			Item(const std::string* s=nullptr, bool mrk=false)
				: symbol{s}, marked{mrk} {}
			const std::string* symbol;
			bool marked;
		};
		std::map<std::string,uint64_t> maps2i_;
		std::vector<Item> symbols_;
		Bag<uint64_t> bag_;
	};

};

#endif
