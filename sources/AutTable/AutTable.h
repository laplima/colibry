//////
//
// AUTTABLE:	Automaton representation in which each state has two
//		sets of transitons: one representing incoming transitions
//		and the other outgoing transitions. Data is duplicated.
//
//		Copyright (C) 1996-2000 by LAPLJ.
//		All rights reserved.
//
//		USES STL LIBRARY
//
//////

#ifndef __AUTTABLE__
#define __AUTTABLE__

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "SymTable.h"
#include "FileException.h"

namespace colibry {

	// General typedefs

	typedef UInt32 StateType;
	typedef UInt32 SigType;

	//
	//	* AUTTABLE INTERFACE
	//

	// ---------- TRANS ----------

	struct TransitionAT {
		TransitionAT();							// Constructor
		TransitionAT(const TransitionAT &);		// Copy constructor
		~TransitionAT();						// Destructor

		SigType    		input;
		std::list<SigType>	outputList;
		StateType		staten;
		bool			mark;

		// Operators
		TransitionAT  &operator=(const TransitionAT &);
		bool operator==(const TransitionAT &) const;
		bool operator<(const TransitionAT &) const;
	};

	// -------- STATE --------

	struct StateAT {
		std::list<TransitionAT> inl;
		std::list<TransitionAT> outl;
	};

	// --------- Automaton exception class definition ----------

	class ExceptionAT : public colibry::Exception {
	public:
		enum Type {
			INVALIDSTN,		// Invalid state number
			INEXISTENT_TRANS,	// Unkown transition
			OUT_OF_RANGE,		// State # < 0 or >MAXSTN
			REPEATED		// Transition already exists in automaton
		};

		ExceptionAT(Type t, const std::string &w);
	};


	/*******************************************************
	 *
	 *				CAUTTABLE CLASS DEFINITION
	 *
	 *******************************************************/

	class AutTable {
	public:

		// Methods
		AutTable(const UInt32 inMaxStates=1);
		AutTable(const std::string &inFileName);
		virtual ~AutTable();

		virtual void LoadFile(const std::string &filename);

		virtual void Add(const StateType is,
				 const SigType in,
				 const std::list<SigType> &out,
				 const StateType fs,
				 const bool inMark=false);

		virtual void Add(const StateType is,
				 const std::string &in,
				 const std::string &out,
				 const StateType fs);

		virtual void RemoveTr(const StateType is,
					  const SigType in,
					  const std::list<SigType> &outlst,
					  const StateType fs);

		virtual void RemoveTr(const StateType is,
					  const std::string &in,
					  const std::string &out,
					  const StateType fs);

		virtual void MarkTr(const StateType is,
					const SigType in,
					const std::list<SigType> &outlst,
					const StateType fs,
					const bool inMark=true);

		virtual void MarkTr(const StateType is,
					const std::string &in,
					const std::string &out,
					const StateType fs,
					const bool inMark=true);

		virtual bool Compress();
		virtual void Clear();

		std::list<TransitionAT>*	GetInl(const StateType staten);
		std::list<TransitionAT>*	GetOutl(const StateType staten);

		UInt32 GetNSt() const { return fNSt; }
		UInt32 GetNTr() const { return fNTr; }
		StateType GetMaxStn() const	{ return fMaxStN; }
		bool IsValidSt(const StateType st) const;

		// Symbol table related static methods

		static std::string	GetSignalName(const SigType inSigIndex);
		static SigType	GetSignalIndex(const std::string &inSigName);
		static bool	IsValidSignal(const std::string &inSigName);
		static bool	IsValidSignal(const SigType inSignal);

		friend std::ostream& operator<<(std::ostream &os, const colibry::AutTable &at);

		static SymTable sIOT;	        // Symbol table

		std::vector<StateAT*> fStt;	// State table vector
		std::vector<StateType> fCompTab;	        // Compress table

	private:

		StateType	fInitState;	// Initial state
		StateType	fCurrent;	// Current state
		UInt32	fNSt;    		// Number of states
		UInt32	fNTr;   		// Number of transitions
		StateType	fMaxStN;	// Maximum state number
		UInt32	fMaxStates;     	// Maximum number of states
	};

}    // colibry namespace


#endif
