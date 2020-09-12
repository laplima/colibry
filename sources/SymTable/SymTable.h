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
//
//     OS: UNIX / WINDOWS
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string>
#include <vector>
#include <climits>
#include <utility>

// Interface

namespace colibry {
    
    typedef unsigned long UInt32;

    // Constants
    
    const UInt32 RESERVED_SIZE = 3096;		// In case of growth
    
    // Errors (exceptions)
    
    struct RangeError {
	UInt32 value;
	std::string where;
	RangeError(const UInt32 v, const std::string &w) { value=v; where=w; }
    };
    


    ////
    // --- TSymTable Class
    ////
    
    class SymTable {
    public:
	
	static const UInt32 NULLINDEX = ULONG_MAX;

    public:
	
	SymTable();                        // Constructor
	virtual ~SymTable();               // Destructor
	
	// Returns the index of an existing symbol, or allocate a new index
	// (and memory) if symbol is not yet in the lookup table.
	// By default, symbols beginning with '*' are marked. Others are not.
	UInt32 LookUp(const std::string &inSymbol) throw (std::bad_alloc);
	
	void Remove(const UInt32 inSigNo) throw (RangeError);
	void Remove(const std::string &inSymbol) throw (RangeError);
	
	UInt32 Find(const std::string &inSymbol) throw();
	
	// Get string from index.
	virtual std::string &GetSymbol(const UInt32 inSigNo) throw (RangeError);
	
	// Empty symbol table freeing all allocated memory.
	void Clear(void) throw();
	
	// Check whether symbol pointed by given index exits.
	bool IsValid(const UInt32 inSigNo) const throw();
	
	// Mark (or unmark) existing symbol. Throws exception, if inSigNo is
	// not valid.
	void Mark(const UInt32 inSigNo, bool inMarked=true) throw (RangeError);
	
	// Check whether symbol is marked.
	bool IsMarked(const UInt32 signo) const throw (RangeError);

	// Get table size
	UInt32 Size();

    private:
	
	// Local structures
	
	struct TItem {
	    std::string	symbol;
	    bool	marked;
	    TItem() {}
	    TItem(const std::string &str, const bool mrk)
	    { symbol = str; marked = mrk; }
	};
	
	std::vector<TItem*> mIOT;

	UInt32 m_size;
    };

}; // namespace


#endif
