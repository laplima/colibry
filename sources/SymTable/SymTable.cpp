//////////////////////////////////////////////////////////////////////////////
//
//  SYMTABLE CLASS IMPLEMENATION
//
//    Copyright (C) 2000 by LAPLJ. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#include "SymTable.h"

using namespace std;
using namespace colibry;

// -------------------------------
// ----- SymTable FUNCTIONS -----
// -------------------------------

SymTable::SymTable()
{
    mIOT.reserve(RESERVED_SIZE);		// Reserve space for growth
    m_size = 0;
}

SymTable::~SymTable()
{
    Clear();
}

// ----- LookUp -------------------------------------------------------
// Return index of the string in the SymTable. Create new, if necessary.

UInt32 SymTable::LookUp(const string &inStr)
    throw (bad_alloc)
{
    UInt32 insertPlace = mIOT.size();		// by default, at the end
    UInt32 sz = insertPlace;

    // Check if signal is already in the table
    // Compute insert place
    UInt32 i,j;
    for (i=0; i<sz; i++)
    	if (mIOT[i] != NULL) {			// may have holes
	    if (mIOT[i]->symbol == inStr)
		return i;
    	} else {
	    // First "hole" found must be the insertPlace
	    insertPlace = i;
	    break;		// For performance
    	}

    // Continue search (avoiding insertPlace computation)
    for (j=i+1; j<sz; j++)
    	if (mIOT[j] != NULL)
	    if (mIOT[j]->symbol == inStr)
		return i;

    if (insertPlace == sz) {
    	// Table needs to grow
    	mIOT.push_back(new TItem(inStr,inStr[0] == '*'));
    	insertPlace = mIOT.size()-1;
    	if (mIOT.capacity() == mIOT.size()) {
	    // Table will be rellocated (reserve next "block")
	    mIOT.reserve(mIOT.size() + RESERVED_SIZE);
    	}
    } else {
    	// Fill hole in table
	mIOT[insertPlace] = new TItem(inStr,inStr[0] == '*');
    }

    m_size++;

    return insertPlace;
}

UInt32 SymTable::Find(const string &inSymbol)
    throw ()
{
    UInt32 sz = mIOT.size();
    for (UInt32 i=0; i<sz; i++)
    	if (mIOT[i] != NULL)
	    if (inSymbol == mIOT[i]->symbol)
		return i;

    return NULLINDEX;
}

void SymTable::Remove(const string &inSymbol)
    throw (RangeError)
{
    Remove(Find(inSymbol));
}

void SymTable::Remove(const UInt32 inSigNo)
    throw (RangeError)
{
    if (!IsValid(inSigNo))
	throw RangeError(inSigNo, "SymTable::Remove()");

    TItem *rem = mIOT[inSigNo];
    mIOT[inSigNo] = NULL;

    m_size--;

    delete rem;
}


string &SymTable::GetSymbol(const UInt32 si)
    throw (RangeError)
    // Better return a copy than a pointer, since the
    // vector could grow and be rellocated.
{
    if (IsValid(si))
	return mIOT[si]->symbol;

    throw RangeError(si, "SymTable::GetSymbol()");
}


void SymTable::Clear(void)
    throw()
{
    for (UInt32 i=0; i<mIOT.size(); i++)
    	if (mIOT[i]) {
	    delete mIOT[i];
	    mIOT[i] = NULL;
    	}
    m_size = 0;
}

bool SymTable::IsValid(const UInt32 signo)
    const throw()
{
    if (signo < mIOT.size())
		return (mIOT[signo] != NULL);
    return false;
}

void SymTable::Mark(const UInt32 inSigNo, bool inMarked)
    throw(RangeError)
{
    if (!IsValid(inSigNo))
	throw RangeError(inSigNo, "SymTable::Mark()");

    mIOT[inSigNo]->marked = inMarked;
}

bool SymTable::IsMarked(const UInt32 signo) const
    throw (RangeError)
{
    // Invalid signal numbers are not marked
    if (!IsValid(signo))
	throw(RangeError(signo, "SymTable::IsMarked()"));

    return mIOT[signo]->marked;
}
