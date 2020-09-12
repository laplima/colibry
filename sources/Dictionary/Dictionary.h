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

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string>
#include <vector>
#include <map>
#include <cinttypes>

// Interface

namespace colibry {

	// --------------------
	// --- Dictionary Class
	// --------------------

	class Dictionary {
	public:

		Dictionary();
		virtual ~Dictionary();

		// accessors
		virtual uint64_t operator[](const std::string& symbol);	// throw if not found
		virtual const std::string& operator[](const uint64_t index);	// idem

		// find/insert
		virtual uint64_t LookUp(const std::string &symbol);

		// Remove will not remove if search is by index
		virtual void Remove(const uint64_t index);
		virtual void Remove(const std::string &symbol);
		virtual void Clear() noexcept;

		// Mark/un-mark existing symbol. Throw exception, if index is invalid
		virtual void Mark(const uint64_t index, bool marked=true);

		// Check whether symbol is marked.
		virtual bool IsMarked(const uint64_t index) const;

		// Get table size
		auto Size() const { return m_symbols.size(); }

	private:

		struct Item {
			Item(const std::string &s, bool mrk=false)
				: symbol(s), marked{mrk} {}
			const std::string& symbol;
			bool marked;
		};
		std::map<std::string,uint64_t> m_s2i;
		std::vector<Item> m_symbols;
		uint64_t m_count;
	};

};

#endif
