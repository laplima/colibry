//////////////////////////////////////////////////////////////////////////////
//
// AUTOMATON INTERFACE DEFINITION
//
//     Copyright (C) 1996-1999 by LAPLJ.
//     All rights reserved.
//
//	Completely dynamic class.
//      OS: UNIX/WINDOWS
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AUTOMATON_H__
#define __AUTOMATON_H__

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <new>
#include "FileException.h"
#include "SymTable.h"

#define MAXLINELEN 1024           // Maximum length of a line in the file


namespace colibry {

    typedef unsigned short UInt16;

    using StateType = std::uint32_t;
    using SigType = std::uint32_t;

    struct Node;

    struct Edge {
		Edge() {}
		Edge(const Edge &);

		bool	operator==(const Edge &ae) const;
		Edge&	operator=(const Edge &ae);

		friend std::ostream& operator<<(std::ostream &os,
						const colibry::Edge &out);

		SigType input;
		SigType output;
		bool isVisited;
		bool isAbsVisited;
		Node *fromNode;
		Node *toNode;
    };


    //
    //	NODE CLASS DEFINITION
    //
    //	Defines a node (or state) in an automaton.
    //
    //	Copyright (C) 2000 by LAPLJ. All rights reserved.
    //


    class Node {
    public:

		Node();
		Node(StateType st);
		Node(const Node &original);

		virtual Node &operator=(const Node &nd);
		virtual bool operator==(const Node &nd) const;

		virtual void AddEdge(Edge &inEdge);
		virtual bool RemoveEdge(Edge &inEdge);

		virtual void Mark(const bool inMark=true) { isVisited = inMark; }
		virtual bool IsMarked() const { return isVisited; }

		StateType get_state_no() const { return stateNo; }
		std::list<Edge>* get_edge_list() { return &edgeList; }
		std::uint32_t get_indegree() const { return inDegree; }

		friend std::ostream &operator<<(std::ostream &os, const Node &nd);

	    private:

		StateType	   stateNo;             // info
		bool		   isVisited;
		std::uint32_t		   inDegree;		// # of incoming transitions
		std::list<Edge>   edgeList;
    };


    //
    // Automaton exception
    //
    class AutException : public Exception {
    public:

	enum Type {
	    INVALID_STATEN,			// Invalid state number
	    INVALID_SIGN,			// Invalid signal number
	    NO_TRANSITION,			// There is no transition with that input
	    NO_UNVISITED_TRANSITION,// There is no unvisited transition with that input
	    EMPTY_AUTOMATON,		// Automaton is empty (#(trans) = #(st) = 0)
	    HAS_ANOTHER_TRANS,		// Delta func. chose between two or more trans.
	    DUPLICATE_TRANS,		// Tried to insert a duplicate transition
	    HAS_HOLES			// Automaton has holes
	};

	AutException(int type, const std::string& where="");
    };

    //
    // CBoolMatrix - Class definition
    //
    //	Boolean matrix.
    //
    //	Copyright (C) 2000 by LAPLJ. All rights reserved.
    //

    // BOOLEAN MATRIX ERRORS (exceptions)

    class BMatException : public Exception {
    public:

	enum Type {
	    BAD_ALLOCATION,
	    RANGE_ERROR,
	    INVALID_SIZE
	};

	BMatException(int type, const std::string& where="",
		      std::uint32_t row=0, std::uint32_t column=0);

    protected:
	std::uint32_t m_row,m_column;
    };

    // ---------------------------------------------------------------------
    //					THE BOOLEAN MATRIX CLASS DEFINITION
    // ---------------------------------------------------------------------

    class BoolMatrix {
    public:

	// Constructor/destructors
	BoolMatrix(const std::uint32_t rows, const std::uint32_t columns);
	BoolMatrix(const BoolMatrix &inMat);
	virtual ~BoolMatrix();

	std::uint32_t GetNRows() const { return mNRows; }
	std::uint32_t GetNCols() const { return mNCols; }

	// Operators
	virtual BoolMatrix& operator=(const BoolMatrix &inMat);

	virtual BoolMatrix& operator|=(const BoolMatrix &inMat);
	virtual BoolMatrix  operator*(const BoolMatrix &inMat);
	virtual bool&        operator()(const std::uint32_t x, const std::uint32_t y);

	friend std::ostream &operator<<(std::ostream &os, const BoolMatrix &bm);

    protected:

	std::uint32_t mNCols, mNRows;

    private:

	bool *mMat;
    };


    ////
    // --- Automaton Class ---
    ////

    class Automaton {
    public:

	//
	// Public attributes
	//

	static SymTable mIOT;        // Symbol table. Static to save memory.


	//
	// Public Methods
	//

	Automaton();
	Automaton(const std::string &inFileName);
	virtual ~Automaton();

	Automaton &operator=(const Automaton &);

	// Adds new transition to automaton.
	virtual void Add(const StateType inInitialState,
			 const SigType inInputSignalNo,
			 const SigType inOutSignalNo,
			 const StateType inTailState);

	virtual void Add(const StateType inInitialState,
			 const std::string &inInputSignal,
			 const std::string &inOutSignal,
			 const StateType inTailState);

	// Reads automaton file in Aldebaran(TM) format.
	virtual void ReadFile(const std::string &inFileName);

	// Clears automaton freeing memory allocated.
	virtual void Clear(void);

	// Removes (existing) transiton from automaton. Throws exception if no
	// such transiton exists.
	virtual void RemoveTr(const StateType is,
			      const std::string &inputSignal,
			      const std::string &outputSignal,
			      const StateType fs);

	// Submits 'in' signal to automaton. May cause a state change and an
	// output 'out' may be produced. Returns the state reached.
	// Exceptions:	NO_TRANSITION - if no NON-VISITED transition
	//				EMPTY_AUTOMATON
	// If successful visits new current state and traversed transition.
	virtual StateType Delta(const std::string &inInputSignal,
				std::string &outOutputSignal);

	// Brings the automaton to its initial state.
	virtual void Reset(void);

	// Checks whether the automaton is deterministic or not.
	virtual bool IsDeterministic(void) const;

	// Checks if given state exists in the automaton.
	virtual bool IsValidSt(const StateType inStateNumber);

	// Returns automaton's current state.
	// Throws EMPTY_AUTOMATON exception
	virtual StateType GetCurrState();

	// Returns automaton intial state.
	// Throws EMPTY_AUTOMATON exception
	virtual StateType GetInitialState();

	// Gets an input signal that can be accepted by the automaton at
	// its current state.
	virtual void GetValidInput(std::string &outSignal); // NOT IMPLEMENTED!!

	// Forces automaton to go to a given (existing) state.
	// Throws EMPTY_AUTOMATON, INVALID_STATEN exceptions.
	virtual void SetCurrState(const StateType stn);

	// Unvisit the state and all outgoing transitons from a that state.
	// Nothing happens when automaton is empty or when inStateNo does
	// no exist.
	virtual void Unvisit(const StateType inStateNo);

	// Unvisit all transitons in the automaton.
	virtual void UnvisitAll(void);

	// True if there are transitions never visited.
	virtual bool HasAbsUnvisited();

	// List transition that were never visited.
	virtual void DisplayAbsUnvisited(std::ostream &os,
					 const std::string &inBeforeEach="");

	// Transitive closure (returns pointer to integer matrix)
	virtual BoolMatrix *GenerateAdjMatrix();
	virtual BoolMatrix *GenerateTClosure();
	virtual std::list< std::list<StateType> > *GetConnectedComponents();

	virtual std::uint32_t GetStateCount() const;	// Returns no. of states in automaton
	virtual std::uint32_t GetTransCount() const;	// Returns no. of transitions in automaton

	// iostream output friend function.
	friend std::ostream& operator<<(std::ostream &os, colibry::Automaton &a);

    protected:

	std::vector<Node*> mStVec;	     // State table (vector)
	Node *mInitialState;
	Node *mCurrState;
	bool mIsDeterministic;		     // Is automaton deterministic?
	std::uint32_t mTransCount;     	     // # of transitions in automaton
	std::uint32_t mStateCount;		     // # of states in automaton
    };

};      // namespace

#endif
