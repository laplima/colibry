///////////////////////////////////////////////////////////////////////////////
//
// SYMTABLE CLASS INTERFACE DEFINITION
//
//     Common Object Library (COLib)
//
//     Version: January 2000
//
//     Description: Instead of dealing with string comparison, assignement,
//                  etc. it is easier and (memory saving) to deal with
//                  indexes uniquely representing these strings.
//                  Conversion from indexes into strings (symbols) and
//                  vice-versa are available whenever needed.
//
//     Copyright (C) 1996-2006 by LAPLJ. All rights reserved.
// 	   (revised in 2020)
//
//     OS: UNIX / WINDOWS
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string>
#include <vector>
#include <cstdint>		// uint32_t
#include <climits>
#include <utility>
#include <stdexcept>
#include <memory>

// Interface

namespace colibry {

	// Errors (exceptions)

	// struct RangeError {
	// 	SymbolID value;
	// 	std::string where;
	// 	RangeError(const SymbolID v, const std::string &w) { value=v; where=w; }
	// };


	////
	// --- TSymTable Class
	////

	class SymTable {
	public:
		class not_found : public std::runtime_error {
		public:
			explicit not_found(const std::string& symbol)
				: std::runtime_error{symbol} {}
		};
	// public:
	// 	static const SymbolID NULLINDEX = ULONG_MAX;

	public:

		SymTable();                        // Constructor
		virtual ~SymTable();               // Destructor

		// Returns the index of an existing symbol, or allocate a new index
		// (and memory) if symbol is not yet in the lookup table.
		// By default, symbols beginning with '*' are marked. Others are not.
		std::uint32_t LookUp(const std::string &inSymbol);

		void Remove(const std::uint32_t inSigNo);
		void Remove(const std::string &inSymbol);

		// may throw not_found
		std::uint32_t Find(const std::string &inSymbol);	// throw if not found
		virtual std::string& GetSymbol(const std::uint32_t inSigNo);	// do not insert

		// Empty symbol table freeing all allocated memory.
		void Clear();

		// Check whether symbol pointed by given index exits.
		bool IsValid(const std::uint32_t inSigNo) const;

		// Mark (or unmark) existing symbol. Throws exception,
		// if inSigNo is not valid.
		void Mark(const std::uint32_t inSigNo, bool inMarked=true);

		// Check whether symbol is marked.
		bool IsMarked(const std::uint32_t signo) const;

		// Get table size
		std::uint32_t Size();

	private:

		// Local structures

		struct TItem {
			std::string symbol;
			bool marked;
			TItem() {}
			TItem(const std::string &str, const bool mrk) : symbol{str}, marked{mrk} {}
		};

		std::vector<std::unique_ptr<TItem>> iot_;
		std::uint32_t size_;
	};

}; // namespace

#endif
