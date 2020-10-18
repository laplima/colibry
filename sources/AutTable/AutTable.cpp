// ==============================================================
//		      CAUTTABLE IMPLEMENTATION
//
//	Copyright (C) 2000 by LAPLJ. All rights reserved.
// ==============================================================

#include "AutTable.h"
#include <fstream>
#include <algorithm>
//#include <cstring>
#include <typeinfo>
#include <sstream>


using namespace colibry;
using namespace std;


// Static member

SymTable AutTable::sIOT;


// MACRO: Throw WRONG_FORMAT exception if pointer is NULL

#define ThrowWFIfNULL(ptr)						\
    if (ptr == nullptr) {							\
	file.close();							\
	throw FileException(FileException::WRONG_FORMAT,		\
			    "AutTable::LoadFile()",			\
			    filename, nline); }

inline void ThrowIfNotFound(string::size_type p, const string& filename,
			    std::uint32_t nline)
{
    if (p == string::npos)
	throw FileException(FileException::WRONG_FORMAT,
			    "AutTable::LoadFile()", filename, nline);
}

inline unsigned long int StrToULong(const string& s)
{
    return strtoul(s.c_str(),(char**)NULL,10);
}

// Constructors

AutTable::AutTable(const std::uint32_t inMaxStates)
{
    fMaxStates = inMaxStates;

    fStt.resize(inMaxStates,NULL);		// Set all elements to NULL
    fCompTab.resize(inMaxStates);

    fInitState = fCurrent = 0;
    fNSt = fNTr = 0;
    fMaxStN = 0;      // -1
}

AutTable::AutTable(const string &filename)
{
    fMaxStates = 0;		// will be changed in LoadFile
    fInitState = fCurrent = 0;
    fNSt = fNTr = 0;
    fMaxStN = 0;

    LoadFile(filename);
}

// --------------------------------------------------------------------------

AutTable::~AutTable()
{
    Clear();
}

// --------------------------------------------------------------------------

void
AutTable::LoadFile(const string &filename)
{
    ifstream file(filename.c_str());
    if (!file.is_open())
    	throw FileException(FileException::FILE_NOT_FOUND,
			     "AutTable::LoadFile()",filename);

    string line;
    string is, label, in, out, fs, aux;
    list<SigType> lout;
    string::size_type p1;

    std::uint32_t nline = 1;

    // AUTOMATON DESCRIPTOR (first line)
    // Get initial state
    getline(file,line);
    p1 = line.find("(");
    ThrowIfNotFound(p1,filename,nline);
    line.erase(0,p1+1);
    p1 = line.find(",");
    ThrowIfNotFound(p1,filename,nline);
    is = line.substr(0,p1);
    line.erase(0,p1+1);

    fInitState = fCurrent = StrToULong(is);

    // Get maximum number of states
    p1 = line.rfind(",");
    ThrowIfNotFound(p1,filename,nline);
    line.erase(0,p1+1);
    p1 = line.find(")");
    ThrowIfNotFound(p1,filename,nline);
    aux = line.substr(0,p1);

    fMaxStates = StrToULong(aux);

    // Resize tables
    fStt.resize(fMaxStates,NULL);
    fCompTab.resize(fMaxStates);

    // PARSE FILE

    nline++;
#if UNIX
    cout << endl;
#endif

    while(!file.eof()) {
	getline(file,line);

#if UNIX
	cout << char(27) << char(77);
	cout << "--- Loading I/O FSM... (line #" << nline << ") " << endl;
#endif


	// ignore empty lines and comments
	if (line.empty() || line.substr(0,1) == "#" || line.substr(0,2) == "//")
	    continue;

	// Get is
	p1 = line.find(",");
	ThrowIfNotFound(p1,filename,nline);
	is = line.substr(1,p1-1);
	line.erase(0,p1+1);

	// Get + parse label
	p1 = line.rfind(",");
	ThrowIfNotFound(p1,filename,nline);
	label = line.substr(0,p1);
	line.erase(0,p1+1);

	// Parse label
	// Get in
	p1 = label.find("/");
	ThrowIfNotFound(p1,filename,nline);
	in = label.substr(1,p1-1);
	label.erase(0,p1+1);
	// Get out
	p1 = label.find("\"");
	ThrowIfNotFound(p1,filename,nline);
	out = label.substr(0,p1);
	// parse out
	lout.clear();
	p1 = out.find(" ");
	while(p1 != string::npos) {
	    lout.push_back(sIOT.LookUp(out.substr(0,p1)));
	    out.erase(0,p1+1);
	    p1 = out.find(" ");
	}
	lout.push_back(sIOT.LookUp(out));

	// Get fs
	p1 = line.find(")");
	ThrowIfNotFound(p1,filename,nline);
	fs = line.substr(0,p1);

	// Create transition
	try {
	    Add(StrToULong(is),sIOT.LookUp(in),lout,StrToULong(fs));
	} catch (ExceptionAT& ae) {
	    if (ae.type() == ExceptionAT::REPEATED) {
		cerr << "Warning: repeated transition! (line #" << nline
		     << ")" << endl;
#if UNIX
		cerr << char(27) << char(77);
#endif
	    } else {
		cerr << filename << " #" << nline << endl;
		throw ExceptionAT((ExceptionAT::Type)ae.type(),
				"AutTable::LoadFile()");
	    }
	}
	nline++;
    }

#if UNIX
    cout << char(27) << char(77);
    cout << "--- Loading I/O FSM... (line #" << nline-1 << ") ";
#endif
    // do not need to close file (destructor will do it)
}

// --------------------------------------------------------------------------

void
AutTable::Add(const StateType is,
	       const SigType in,
	       const list<SigType> &out,
	       const StateType fs,
               const bool inMark)
{
    if (is >= fMaxStates || fs >= fMaxStates /*|| is < 0 || fs < 0*/)
    	throw ExceptionAT(ExceptionAT::OUT_OF_RANGE,"AutTable::Add()");

    if (is > fMaxStN) fMaxStN = is;
    if (fs > fMaxStN) fMaxStN = fs;

    // Create new state cell if it doesn't already exist
    if (fStt[is] == NULL) {
		fStt[is] = new StateAT;
		fCompTab[is] = is;
		fNSt++;
    }

    // Create new state cell if it doesn't already exist
    if (fStt[fs] == NULL) {
		fStt[fs] = new StateAT;
		fCompTab[fs] = fs;
		fNSt++;
    }

    // Instantiate transition
    TransitionAT *tr = new TransitionAT;
    tr->input = in;
    tr->outputList = out;
    tr->staten = fs;
    tr->mark = inMark;

    // Add to out list from state is
    list<TransitionAT> &ol = fStt[is]->outl;		// shorthand
    if (find(ol.begin(), ol.end(), *tr) == ol.end()) {
    	// *tr not found in outl from state is
    	ol.push_back(*tr);
    } else {
    	// repeated transition - discard it
		delete tr;
    	throw ExceptionAT(ExceptionAT::REPEATED,"AutTable::Add()");
    }

	// Add to in list of state fs
    tr->staten = is;
    list<TransitionAT> &il = fStt[fs]->inl;		// shorthand
    if (find(il.begin(), il.end(), *tr) == il.end()) {
    	// * tr not found in inl of state fs
    	il.push_back(*tr);
    } else {
    	// Repeated transition
		delete tr;
    	throw(ExceptionAT(ExceptionAT::REPEATED,"AutTable::Add()"));
    }

    delete tr;
    fNTr++;
}

// --------------------------------------------------------------------------

void
AutTable::Add(const StateType is,
			   const string &in,
			   const string &out,
			   const StateType fs)
{
    SigType ins, ous;
	ins = sIOT.LookUp(in);
	ous = sIOT.LookUp(out);

    list<SigType> lout;
    lout.push_back(ous);

    Add(is, ins, lout, fs);		// May throw memory, automaton exception
}

// --------------------------------------------------------------------------

void
AutTable::RemoveTr(const StateType is,
					const SigType in,
					const list<SigType> &out,
					const StateType fs)
{
    if (is >= fMaxStates || fs >= fMaxStates/* || fs < 0 || is < 0*/) // unsigned!!
    	throw ExceptionAT(ExceptionAT::OUT_OF_RANGE,"AutTable::RemoveTr()");

    TransitionAT tr;
    tr.input = in;
    tr.outputList = out;
    tr.staten = fs;

    // Erase out transition from state is
    list<TransitionAT> &ol = fStt[is]->outl;
    list<TransitionAT>::iterator it = find(ol.begin(), ol.end(), tr);
    if (it != ol.end())
    	ol.erase(it);	// erase found transition
    else
    	throw ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::RemoveTr()");

    // Erase in transition to state fs
    tr.staten = is;
    list<TransitionAT> &il = fStt[fs]->inl;
    it = find(il.begin(), il.end(), tr);
    if (it != il.end())
    	il.erase(it);	// erase found transition
    else
    	throw ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::RemoveTr()");

	// Remove states if in & out lists are empty
    if (fStt[is]->inl.empty() && fStt[is]->outl.empty()) {
		// Remove state
		delete fStt[is];
		fStt[is] = NULL;
		fNSt--;
    }
    if (fStt[fs]->inl.empty() && fStt[fs]->outl.empty()) {
		// Remove state
		delete fStt[fs];
		fStt[fs] = NULL;
		fNSt--;
    }

    fNTr--;
}

// --------------------------------------------------------------------------

void
AutTable::RemoveTr(const StateType is,
					const string &in,
					const string &out,
					const StateType fs)
{
    SigType ins, ous;
    try {
		ins = sIOT.LookUp(in);
		ous = sIOT.LookUp(out);
    } catch (...) {
    	// Transform memory exception in automaton exception
    	throw(ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::RemoveTr()"));
    }

    list<SigType> lout;
    lout.push_back(ous);
    RemoveTr(is,ins,lout,fs);
}

// -------------------------------------------------------------------------

void
AutTable::MarkTr(const StateType is,
				  const SigType in,
				  const list<SigType> &out,
				  const StateType fs,
				  const bool inMark)
{
    if (is >= fMaxStates || fs >= fMaxStates /*|| fs < 0 || is < 0*/)
    	throw ExceptionAT(ExceptionAT::OUT_OF_RANGE,"AutTable::MarkTr()");

    TransitionAT aTR;
    aTR.staten = fs;
    aTR.input = in;
    aTR.outputList = out;

    list<TransitionAT> &ol = fStt[is]->outl;
    list<TransitionAT>::iterator it = find(ol.begin(), ol.end(), aTR);
    if (it == ol.end())
    	// aTR not found
    	throw ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::MarkTr()");

    it->mark = inMark;

    aTR.staten = is;
	list<TransitionAT> &il = fStt[fs]->inl;
	it = find(il.begin(),il.end(),aTR);
	if (it == il.end())
		// aTR not found
    	throw(ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::MarkTr()"));

    it->mark = inMark;
}


void
AutTable::MarkTr(const StateType is,
				  const string &in,
				  const string &out,
				  const StateType fs,
				  const bool inMark)
{
    SigType ins, ous;
    try {
		ins = sIOT.LookUp(in);
		ous = sIOT.LookUp(out);
    } catch (...) {
    	// Transform memory exception in automaton exception
    	throw(ExceptionAT(ExceptionAT::INEXISTENT_TRANS,"AutTable::RemoveTr()"));
    }

    list<SigType> lout;
    lout.push_back(ous);
    MarkTr(is,ins,lout,fs,inMark);
}


// --------------------------------------------------------------------------


bool
AutTable::Compress()
{
    bool sdone = false;
    StateType stnum = 0;

    // Compute compression table
    for (StateType i=0; i<=fMaxStN; i++)
		if (!IsValidSt(i))
		    sdone = true;
		else
			fCompTab[i]=stnum++;

    return sdone;
}


// --------------------------------------------------------------------------


list<TransitionAT> *
AutTable::GetInl(const StateType st)
{
    if (!IsValidSt(st))
		return NULL;

    return &(fStt[st]->inl);
}

// --------------------------------------------------------------------------

list<TransitionAT> *
AutTable::GetOutl(const StateType st)
{
    if (!IsValidSt(st))
		return NULL;

    return &(fStt[st]->outl);
}

// --------------------------------------------------------------------------

void
AutTable::Clear()
{
    for (StateType i=0; i<=fMaxStN; i++)
	if (fStt[i] != NULL) {
	    delete fStt[i];
	    fStt[i] = NULL;
	}

    fInitState = fCurrent = 0;
    fNSt = fNTr = 0;
    fMaxStN = 0;
}

bool
AutTable::IsValidSt(StateType st) const
{
    if (st>fMaxStN)		// Will never be < 0
		return false;

    return (bool)(fStt[st] != NULL);
}


// Symbol table related static methods

string
AutTable::GetSignalName(const SigType inSigIndex)
{
    return sIOT.GetSymbol(inSigIndex);
}

SigType
AutTable::GetSignalIndex(const string &inSigName)
{
    return sIOT.LookUp(inSigName);
}

bool
AutTable::IsValidSignal(const string &inSigName)
{
    try {
        sIOT.Find(inSigName);
        return true;
    } catch(const SymTable::not_found&) {
        return false;
    }
}

bool
AutTable::IsValidSignal(const SigType inSignal)
{
	return sIOT.IsValid(inSignal);
}


// IOSTREAM

ostream& colibry::operator<<(ostream &os, const colibry::AutTable &at)
{
    // Descriptor
    os << "des(0," << at.GetNTr() << "," << at.GetNSt() << ")" << endl;

    // TransitionATs
    list<TransitionAT>::iterator	itT;
    list<SigType>::iterator		itS;
    for (StateType i=0; i<=at.GetMaxStn(); i++) {

	if (!at.IsValidSt(i))
	    continue;

	for (itT = at.fStt[i]->outl.begin(); itT != at.fStt[i]->outl.end(); itT++) {
	    // is + input
	    os << "(" << at.fCompTab[i] << ",\""
               << AutTable::GetSignalName(itT->input) << "/";
            // outputList
	    itS = itT->outputList.begin();
	    if (itS != itT->outputList.end())
		os << AutTable::GetSignalName(*itS);	// First element
	    for (itS++; itS != itT->outputList.end(); itS++)
		os << " " << AutTable::GetSignalName(*itS);
	    // staten
	    os << "\"," << at.fCompTab[itT->staten] << ")" << endl;
	}
    }
    return os;
}

#pragma mark -
#pragma mark === TRANSITION ===

//------------------------------------------------------------
//
//						TRANSITION
//
//------------------------------------------------------------

TransitionAT::TransitionAT()
{
    mark = false;
}

TransitionAT::TransitionAT(const TransitionAT &original)
{
    input = original.input;
    outputList = original.outputList;
    staten = original.staten;
    mark = original.mark;
}

TransitionAT::~TransitionAT()
{
}

//////
//
// OPERATORS
//
//////

TransitionAT &TransitionAT::operator=(const TransitionAT &t)
{
	if (&t != this) {
	    input = t.input;
	    outputList = t.outputList;
	    staten = t.staten;
	    mark = t.mark;
	}
    return *this;
}

bool TransitionAT::operator==(const TransitionAT &t) const
{
    return (bool)(input==t.input && outputList==t.outputList
    				&& staten==t.staten);
}

bool TransitionAT::operator< (const TransitionAT &t) const
{
    return (bool)(staten < t.staten);
}

#pragma mark -
#pragma mark === EXCEPTION ===

//------------------------------------------------------------
//
//					AUTOMATON EXCEPTION
//
//------------------------------------------------------------

ExceptionAT::ExceptionAT(Type t, const string& inWhere)
    : Exception(int(t),inWhere)
{
    switch (mType) {
    case INVALIDSTN:
	mWhat = "Invalid state number";
	break;
    case INEXISTENT_TRANS:
	mWhat = "Inexistent transition in automaton";
	break;
    case OUT_OF_RANGE:
	mWhat = "State number out of range (check des(_,_,_))";
	break;
    case REPEATED:
	mWhat = "Repeated transition in automaton";
	break;
    default:
	mWhat = "Unknown exception";
    }
}
