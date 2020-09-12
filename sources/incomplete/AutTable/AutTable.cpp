// ==============================================================
//		      CAUTTABLE IMPLEMENTATION
//	
//	Copyright (C) 2000 by LAPLJ. All rights reserved.
// ==============================================================

#include "CAutTable.h"
#include <fstream>
#include <algorithm>
//#include <cstring>
#include <typeinfo>
#include <sstream>


using namespace AutTable;
using namespace COLib;


// Static member

CSymTable CAutTable::sIOT;


// MACRO: Throw WRONG_FORMAT exception if pointer is NULL

#define ThrowWFIfNULL(ptr)					              \
	if (ptr == NULL) {						      \
		file.close();   					      \
		throw CFileException(CFileException::WRONG_FORMAT,	      \
				     "CAutTable::LoadFile()",		      \
				     filename, nline); }

inline void ThrowIfNotFound(string::size_type p, const string& filename,
			    UInt32 nline)
    throw (CFileException)
{
    if (p == string::npos)
	throw CFileException(CFileException::WRONG_FORMAT,
			     "CAutTable::LoadFile()", filename, nline);
}

inline unsigned long int StrToULong(const string& s)
{
    return strtoul(s.c_str(),(char**)NULL,10);
}
			     

// Constructors

CAutTable::CAutTable(const UInt32 inMaxStates)
{
    fMaxStates = inMaxStates;
    
    fStt.resize(inMaxStates,NULL);		// Set all elements to NULL
    fCompTab.resize(inMaxStates);
	
    fInitState = fCurrent = 0;
    fNSt = fNTr = 0;
    fMaxStN = 0;      // -1
}

CAutTable::CAutTable(const string &filename)
    throw(CFileException,Exception,bad_alloc)
{
    fMaxStates = 0;		// will be changed in LoadFile
    fInitState = fCurrent = 0;
    fNSt = fNTr = 0;
    fMaxStN = 0;

    LoadFile(filename);
}

// --------------------------------------------------------------------------

CAutTable::~CAutTable()
{
    Clear();
}

// --------------------------------------------------------------------------

void
CAutTable::LoadFile(const string &filename)
    throw(CFileException,Exception,bad_alloc)
{
    ifstream file(filename.c_str());
    if (!file.is_open())
    	throw CFileException(CFileException::FILE_NOT_FOUND,
			     "CAutTable::LoadFile()",filename);

    string line;
    string is, label, in, out, fs, aux;
    list<SigType> lout;
    string::size_type p1;

    UInt32 nline = 1;

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
	} catch (Exception& ae) {
	    if (ae.type() == Exception::REPEATED) {
		cerr << "Warning: repeated transition! (line #" << nline
		     << ")" << endl;
#if UNIX
		cerr << char(27) << char(77);
#endif
	    } else {
		cerr << filename << " #" << nline << endl;
		throw Exception((Exception::Type)ae.type(),
				"CAutTable::LoadFile()");
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
CAutTable::Add(const StateType is,
	       const SigType in,
	       const list<SigType> &out,
	       const StateType fs,
               const bool inMark) throw(bad_alloc,Exception)
{
    if (is >= fMaxStates || fs >= fMaxStates || is < 0 || fs < 0)
    	throw Exception(Exception::OUT_OF_RANGE,"CAutTable::Add()");

    if (is > fMaxStN) fMaxStN = is;
    if (fs > fMaxStN) fMaxStN = fs;

    // Create new state cell if it doesn't already exist
    if (fStt[is] == NULL) {
		fStt[is] = new State;
		fCompTab[is] = is;
		fNSt++;
    }

    // Create new state cell if it doesn't already exist
    if (fStt[fs] == NULL) {
		fStt[fs] = new State;
		fCompTab[fs] = fs;
		fNSt++;
    }

    // Instantiate transition
    Transition *tr = new Transition;
    tr->input = in;
    tr->outputList = out;
    tr->staten = fs;
    tr->mark = inMark;
    
    // Add to out list from state is
    list<Transition> &ol = fStt[is]->outl;		// shorthand
    if (find(ol.begin(), ol.end(), *tr) == ol.end()) {
    	// *tr not found in outl from state is
    	ol.push_back(*tr);
    } else {
    	// repeated transition - discard it
		delete tr;
    	throw Exception(Exception::REPEATED,"CAutTable::Add()");
    }

	// Add to in list of state fs
    tr->staten = is;
    list<Transition> &il = fStt[fs]->inl;		// shorthand
    if (find(il.begin(), il.end(), *tr) == il.end()) {
    	// * tr not found in inl of state fs
    	il.push_back(*tr);
    } else {
    	// Repeated transition
		delete tr;
    	throw(Exception(Exception::REPEATED,"CAutTable::Add()"));
    }

    delete tr;
    fNTr++;
}

// --------------------------------------------------------------------------

void
CAutTable::Add(const StateType is,
			   const string &in,
			   const string &out,
			   const StateType fs)	throw(bad_alloc, Exception)
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
CAutTable::RemoveTr(const StateType is,
					const SigType in,
					const list<SigType> &out,
					const StateType fs)		throw(Exception)
{
    if (is >= fMaxStates || fs >= fMaxStates || fs < 0 || is < 0)
    	throw Exception(Exception::OUT_OF_RANGE,"CAutTable::RemoveTr()");
    
    Transition tr;
    tr.input = in;
    tr.outputList = out;
    tr.staten = fs;
    
    // Erase out transition from state is
    list<Transition> &ol = fStt[is]->outl;
    list<Transition>::iterator it = find(ol.begin(), ol.end(), tr);
    if (it != ol.end())
    	ol.erase(it);	// erase found transition
    else
    	throw Exception(Exception::INEXISTENT_TRANS,"CAutTable::RemoveTr()");
    
    // Erase in transition to state fs
    tr.staten = is;
    list<Transition> &il = fStt[fs]->inl;
    it = find(il.begin(), il.end(), tr);
    if (it != il.end())
    	il.erase(it);	// erase found transition
    else
    	throw Exception(Exception::INEXISTENT_TRANS,"CAutTable::RemoveTr()");

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
CAutTable::RemoveTr(const StateType is,
					const string &in,
					const string &out,
					const StateType fs)	throw(Exception)
{
    SigType ins, ous;
    try {
		ins = sIOT.LookUp(in);
		ous = sIOT.LookUp(out);
    } catch (...) {
    	// Transform memory exception in automaton exception
    	throw(Exception(Exception::INEXISTENT_TRANS,"CAutTable::RemoveTr()"));
    }

    list<SigType> lout;
    lout.push_back(ous);
    RemoveTr(is,ins,lout,fs);
}

// -------------------------------------------------------------------------

void
CAutTable::MarkTr(const StateType is,
				  const SigType in,
				  const list<SigType> &out,
				  const StateType fs,
				  const bool inMark)	throw(Exception)
{
    if (is >= fMaxStates || fs >= fMaxStates || fs < 0 || is < 0)
    	throw Exception(Exception::OUT_OF_RANGE,"CAutTable::MarkTr()");

    Transition aTR;
    aTR.staten = fs;
    aTR.input = in;
    aTR.outputList = out;
    
    list<Transition> &ol = fStt[is]->outl;
    list<Transition>::iterator it = find(ol.begin(), ol.end(), aTR);
    if (it == ol.end())
    	// aTR not found
    	throw Exception(Exception::INEXISTENT_TRANS,"CAutTable::MarkTr()");
    
    it->mark = inMark;

    aTR.staten = is;
	list<Transition> &il = fStt[fs]->inl;
	it = find(il.begin(),il.end(),aTR);
	if (it == il.end())
		// aTR not found
    	throw(Exception(Exception::INEXISTENT_TRANS,"CAutTable::MarkTr()"));
    
    it->mark = inMark;
}


void
CAutTable::MarkTr(const StateType is,
				  const string &in,
				  const string &out,
				  const StateType fs,
				  const bool inMark)	throw(Exception)
{
    SigType ins, ous;
    try {
		ins = sIOT.LookUp(in);
		ous = sIOT.LookUp(out);
    } catch (...) {
    	// Transform memory exception in automaton exception
    	throw(Exception(Exception::INEXISTENT_TRANS,"CAutTable::RemoveTr()"));
    }

    list<SigType> lout;
    lout.push_back(ous);
    MarkTr(is,ins,lout,fs,inMark);
}


// --------------------------------------------------------------------------


bool
CAutTable::Compress()
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


list<Transition> *
CAutTable::GetInl(const StateType st)
{
    if (!IsValidSt(st))
		return NULL;

    return &(fStt[st]->inl);
}

// --------------------------------------------------------------------------

list<Transition> *
CAutTable::GetOutl(const StateType st)
{
    if (!IsValidSt(st))
		return NULL;

    return &(fStt[st]->outl);
}

// --------------------------------------------------------------------------

void
CAutTable::Clear()
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
CAutTable::IsValidSt(StateType st) const
{
    if (st>fMaxStN)		// Will never be < 0
		return false;

    return (bool)(fStt[st] != NULL);
}


// Symbol table related static methods

string 
CAutTable::GetSignalName(const SigType inSigIndex)
{
    return sIOT.GetSymbol(inSigIndex);
}

SigType
CAutTable::GetSignalIndex(const string &inSigName)
{
    return sIOT.LookUp(inSigName);
}

bool
CAutTable::IsValidSignal(const string &inSigName)
{
    return (bool)(sIOT.Find(inSigName) != CSymTable::NULLINDEX);
}

bool
CAutTable::IsValidSignal(const SigType inSignal)
{
	return sIOT.IsValid(inSignal);
}


// IOSTREAM

ostream& operator<<(ostream &os, const CAutTable &at)
{
	// Descriptor
    os << "des(0," << at.fNTr << "," << at.fNSt << ")" << endl;
    
    // Transitions
    list<Transition>::iterator	itT;
    list<SigType>::iterator		itS;
    for (StateType i=0; i<=at.fMaxStN; i++) {
	
		if (!at.IsValidSt(i))
			continue;
		
		for (itT = at.fStt[i]->outl.begin(); itT != at.fStt[i]->outl.end(); itT++) {
			// is + input
			os << "(" << at.fCompTab[i] << ",\""
               << CAutTable::GetSignalName(itT->input) << "/";
            // outputList
			itS = itT->outputList.begin();
			if (itS != itT->outputList.end())
				os << CAutTable::GetSignalName(*itS);	// First element
			for (itS++; itS != itT->outputList.end(); itS++)
				os << " " << CAutTable::GetSignalName(*itS);
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

Transition::Transition()
{
    mark = false;
}

Transition::Transition(const Transition &original)
{
    input = original.input;
    outputList = original.outputList;
    staten = original.staten;
    mark = original.mark;
}

Transition::~Transition()
{
}

//////
//
// OPERATORS
//
//////

Transition &Transition::operator=(const Transition &t)
{
	if (&t != this) {
	    input = t.input;
	    outputList = t.outputList;
	    staten = t.staten;
	    mark = t.mark;
	}
    return *this;
}

bool Transition::operator==(const Transition &t) const
{
    return (bool)(input==t.input && outputList==t.outputList
    				&& staten==t.staten);
}

bool Transition::operator< (const Transition &t) const
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

Exception::Exception(Type t, const string& inWhere)
    : CException(int(t),inWhere)
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
