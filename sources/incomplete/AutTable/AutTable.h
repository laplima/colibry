//////
//
// AUTTABLE:	Automaton representation in which each state has two
//		sets of transitons: one representing incoming transitions
//		and the other outgoing transitions. Data is duplicated.
//
//		Copyright (C) 1996-2006 by LAPLJ.
//		All rights reserved.
//
//		USES STL LIBRARY
//
//   SO: UNIX / WINDOWS
//
//////

#ifndef __AUTTABLE_H__
#define __AUTTABLE_H__

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <SymTable.h>
#include <FileException.h>

using std::vector;
using std::list;
using std::string;

namespace NCOLib {

    namespace AutTableNS {
    

	// General typedefs
	
	typedef UInt32 StateType;
	typedef UInt32 SigType;

	//
	//	* AUTTABLE INTERFACE
	//
	

	// ---------- TRANS ----------
    
	struct Transition
	{
	    Transition();						// Constructor
	    Transition(const Transition &);		// Copy constructor
	    ~Transition();						// Destructor
	    
	    SigType    		input;
	    list<SigType>	outputList;
	    StateType		staten;
	    bool		mark;
	
	    // Operators
	    Transition  &operator=(const Transition &);
	    bool operator==(const Transition &) const;
	    bool operator<(const Transition &) const;
	};
    
	// -------- STATE --------
	
	struct State {
	    list<Transition> inl;
	    list<Transition> outl;
	};
    
	// --------- Automaton exception class definition ----------
	
	class ATException : public ::NCOLib::Exception {
	public:
	    enum Type {
		INVALIDSTN,		// Invalid state number
		INEXISTENT_TRANS,	// Unkown transition
		OUT_OF_RANGE,		// State # < 0 or >MAXSTN
		REPEATED		// Transition already exists in automaton
	    };
	
	    ATException(Type t, const string &w);
	};
    
    };	// namespace AutTable


    /*******************************************************
     *
     *				CAUTTABLE CLASS DEFINITION
     *
     *******************************************************/
    
    class AutTable {
    public:
	
	// Methods
	AutTable(const UInt32 inMaxStates=1);
	AutTable(const string &inFileName)
	    throw(COLib::CFileException,AutTable::Exception,bad_alloc);
    virtual ~AutTable();

    virtual void LoadFile(const string &filename)
	throw(COLib::CFileException,AutTable::Exception,bad_alloc);
    					
    virtual void Add(const StateType is,
		     const SigType in,
		     const list<SigType> &out,
		     const StateType fs,
		     const bool inMark=false)
	throw(bad_alloc,AutTable::Exception);
    				
    virtual void Add(const StateType is,
		     const string &in,
		     const string &out,
		     const StateType fs)
	throw(bad_alloc,AutTable::Exception);
    
    virtual void RemoveTr(const StateType is,
			  const SigType in,
			  const list<SigType> &outlst,
			  const StateType fs)
	throw(AutTable::Exception);
    							
    virtual void RemoveTr(const StateType is,
			  const string &in,
			  const string &out,
			  const StateType fs)
	throw(AutTable::Exception);
    
    virtual void MarkTr(const StateType is,
			const SigType in,
			const list<SigType> &outlst,
			const StateType fs,
			const bool inMark=true)
	throw(AutTable::Exception);
    
    virtual void MarkTr(const StateType is,
			const string &in,
			const string &out,
			const StateType fs,
			const bool inMark=true)
	throw(AutTable::Exception);

    virtual bool Compress();
    virtual void Clear();
    
    list<AutTable::Transition>*	GetInl(const StateType staten);
    list<AutTable::Transition>*	GetOutl(const StateType staten);
    
    UInt32 GetNSt() const { return fNSt; }
    UInt32 GetNTr() const { return fNTr; }
    StateType GetMaxStn() const	{ return fMaxStN; }
    bool IsValidSt(const StateType st) const;

    // Symbol table related static methods
	
    static string	GetSignalName(const SigType inSigIndex);
    static SigType	GetSignalIndex(const string &inSigName);
    static bool		IsValidSignal(const string &inSigName);
    static bool		IsValidSignal(const SigType inSignal);

    friend ostream& operator<<(ostream &os, const AutTable &at);

private:

    static COLib::CSymTable sIOT;	// Symbol table
    
    vector<AutTable::State*> fStt;	// State table vector
    vector<StateType> fCompTab;	        // Compress table
    
    StateType	fInitState;		// Initial state
    StateType	fCurrent;		// Current state
    UInt32	fNSt;    		// Number of states
    UInt32	fNTr;   		// Number of transitions
    StateType	fMaxStN;		// Maximum state number
    UInt32	fMaxStates;     	// Maximum number of states
};


#endif
