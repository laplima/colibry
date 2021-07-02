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

SymTable::SymTable() : size_{0}
{
	// const unsigned int resource_allocation = 4096;
	// iot_.reserve(resource_allocation);		// Reserve space for growth
}

// ----- LookUp -------------------------------------------------------
// Return index of the string in the SymTable. Create new, if necessary.

uint32_t SymTable::LookUp(const string &inStr)
{
	uint32_t insertPlace = iot_.size();		// by default, at the end
	const uint32_t sz = insertPlace;

	// Check if signal is already in the table
	// Compute insert place
	uint32_t i;
	for (i=0; i<sz; ++i) {
		if (iot_[i]) {			// may have holes
			if (iot_[i]->symbol == inStr) return i;
		} else {
			// First "hole" found must be the insertPlace
			insertPlace = i;
			break;		// For performance
		}
	}

	// Continue search (avoiding insertPlace computation)
	for (++i; i<sz; ++i)
		if (iot_[i])
			if (iot_[i]->symbol == inStr)
				return i;

	// not found: insert

	if (insertPlace == sz) {
		// Table needs to grow
		iot_.emplace_back(make_unique<TItem>(inStr, inStr[0] == '*'));
		insertPlace = sz;
	} else {
		// Fill hole in table
		iot_[insertPlace] = make_unique<TItem>(inStr,inStr[0] == '*');
	}

	++size_;
	return insertPlace;
}

uint32_t SymTable::Find(const string &inSymbol)
{
	uint32_t sz = iot_.size();
	for (uint32_t i=0; i<sz; ++i)
		if (iot_[i])
			if (inSymbol == iot_[i]->symbol)
				return i;
	throw not_found{inSymbol};
}

void SymTable::Remove(const string &inSymbol)
{
	Remove(Find(inSymbol));
}

void SymTable::Remove(const uint32_t inSigNo)
{
	if (!IsValid(inSigNo))
		throw not_found(to_string(inSigNo));

	iot_[inSigNo].reset();
	--size_;
}

string &SymTable::GetSymbol(const uint32_t si)
	// Better return a copy than a pointer, since the
	// vector could grow and be rellocated.
{
	if (IsValid(si))
		return iot_[si]->symbol;
	throw not_found{to_string(si)};
}

void SymTable::Clear()
{
	iot_.clear();
	size_ = 0;
}

bool SymTable::IsValid(const uint32_t signo) const
{
	if (signo < iot_.size())
		return static_cast<bool>(iot_[signo]);
	return false;
}

void SymTable::Mark(const uint32_t inSigNo, bool inMarked)
{
	if (!IsValid(inSigNo))
		throw not_found(to_string(inSigNo));

	iot_[inSigNo]->marked = inMarked;
}

bool SymTable::IsMarked(const uint32_t signo) const
{
	// Invalid signal numbers are not marked
	if (!IsValid(signo))
		throw not_found(to_string(signo));

	return iot_[signo]->marked;
}
