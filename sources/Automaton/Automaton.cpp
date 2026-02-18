//////////////////////////////////////////////////////////////////////////////
//
// AUTOMATON IMPLEMENTATION
//
// Copyright 1996-1997 by LAPLJ. All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>
// #include <sstream>
#include <throw_if.h>
#include "Automaton.h"

using namespace std;
using namespace colibry;

// Type defintions
using Line = char[MAXLINELEN];
// typedef char Line[MAXLINELEN];

constexpr uint32_t AUTRESERVED_SIZE = 128;			// Reserved size for state vector

// Macros
// #define ThrowWFIfNULL(ptr)	\
// 	if (ptr == nullptr) {					\
// 		file.close();					\
// 		throw FileException(FileException::WRONG_FORMAT,\
// 			"Automaton::ReadFile()", file_name,ln); }


// Helpful functions prototypes
bool CompareLines(const BoolMatrix *bm, uint32_t i, uint32_t j);


//
// EDGE IMPLEMENTATION
//

Edge::Edge(const Edge& ae)
{
    input = ae.input;
    output = ae.output;
    isVisited = ae.isVisited;
    isAbsVisited = ae.isAbsVisited;
    toNode = ae.toNode;
    fromNode = ae.fromNode;
}

bool Edge::operator==(const Edge &ae) const
{
    return (input == ae.input && output == ae.output &&
	    toNode == ae.toNode && fromNode == ae.fromNode);
}

Edge &Edge::operator=(const Edge &ae)
{
    if (&ae == this)
    	return *this;

    input = ae.input;
    output = ae.output;
    isVisited = ae.isVisited;
    isAbsVisited = ae.isAbsVisited;
    toNode = ae.toNode;
    fromNode = ae.fromNode;
    return (*this);
}

std::ostream& operator<<(std::ostream &os, const Edge &out)
{
    return (os	<< "(" << out.fromNode->get_state_no() << ","
	    << out.input << "/" << out.output << ","
	    << out.toNode->get_state_no() << ")" << std::endl );
}

//
// NODE IMPLEMENTATION
//

Node::Node()
{
    stateNo = 0;
    isVisited = false;
    inDegree = 0;
}

Node::Node(StateType st)
{
    stateNo = st;
    isVisited = false;
    inDegree = 0;
}

Node::Node(const Node &node)
{
    *this = node;
}

Node &Node::operator=(const Node &nd)
{
    if (&nd == this)
	   return *this;

    stateNo = nd.stateNo;
    edgeList = nd.edgeList;
    isVisited = nd.isVisited;
    inDegree = nd.inDegree;
    return *this;
}

bool Node::operator==(const Node &nd) const
{
    return (bool)(stateNo == nd.stateNo);
}

void Node::AddEdge(Edge &inEdge)
{
    inEdge.fromNode = this;
    edgeList.push_back(inEdge);
    edgeList.unique();			// remove repeated elements
    if (inEdge.toNode != nullptr)
	inEdge.toNode->inDegree++;
}

bool Node::RemoveEdge(Edge &inEdge)
{
    inEdge.fromNode = this;
    list<Edge>::iterator it;
    it = find(edgeList.begin(),edgeList.end(),inEdge);
    if (it == edgeList.end())
	return false;

    edgeList.erase(it);
    if (inEdge.toNode != nullptr)
	inEdge.toNode->inDegree--;
    return true;
}

ostream &colibry::operator<<(ostream &os, const Node &nd)
{
    return (os << nd.get_state_no());
}

//
// BOOLMATRIX IMPLEMENTATION
//

//
// BMatException
//

/*
BMatException::BMatException(int type, const string& where,
			       uint32_t row, uint32_t col)
    : Exception((int)type,where), m_row(row), m_column(col)
{
    ostringstream ss;
    switch (static_cast<Type>(mType)) {
    using enum Type;
    case BAD_ALLOCATION:
    	mWhat = "Bad allocation - unsufficient memory.";
    	break;
    case RANGE_ERROR:
    	ss << "Range error [" << m_row << "][" << m_column << "].";
    	mWhat = ss.str();
    	break;
    case INVALID_SIZE:
    	mWhat = "Invalid size.";
    	break;
    default:
    	mWhat = "Unknown exception.";
    }
}
*/
//
// BoolMatrix
//

BoolMatrix::BoolMatrix(uint32_t ln, uint32_t rw)
{
    try {
    	mNRows = ln;
    	mNCols = rw;
    	mMat = new bool[mNRows*mNCols];
    } catch (const bad_alloc &ba) {
        throw runtime_error{"BAD_ALLOCATION"};
    	// throw BMatException(BMatException::Type::BAD_ALLOCATION,
    	// 		     "BoolMatrix::BoolMatrix()");
    }
}

BoolMatrix::BoolMatrix(const BoolMatrix &inMat)
{
    mMat = nullptr;
    *this = inMat;
}

BoolMatrix::~BoolMatrix()
{
    if (mMat != nullptr)
	   delete [] mMat;
}

BoolMatrix &BoolMatrix::operator|=(const BoolMatrix &inMat)
{
    if (mNCols != inMat.mNCols || mNRows != inMat.mNRows)
        throw runtime_error{"INVALID_SIZE"};
	// throw BMatException(BMatException::INVALID_SIZE,
	// 		     "BoolMatrix::operator|=()");

    for (uint32_t i=0; i<mNRows*mNCols; i++)
    	mMat[i] = mMat[i] || inMat.mMat[i];
    return (*this);
}

bool &BoolMatrix::operator()(uint32_t x, uint32_t y)
{
    if (x>=mNRows || y>=mNCols)
        throw runtime_error{"RANGE_ERROR"};
	// throw BMatException(BMatException::RANGE_ERROR,
	// 		     "BoolMatrix::operator()", x,y);

    return mMat[x*mNRows+y];
}

bool& BoolMatrix::operator()(std::uint32_t x, std::uint32_t y) const
{
    if (x>=mNRows || y>=mNCols)
        throw runtime_error{"RANGE_ERROR"};
    // throw BMatException(BMatException::RANGE_ERROR,
    //           "BoolMatrix::operator()", x,y);

    return mMat[x*mNRows+y];
}

BoolMatrix BoolMatrix::operator*(const BoolMatrix &a)
{
    // Check matrix dimensions
    if (mNCols != a.mNRows || mNRows != a.mNCols)
        throw runtime_error{"INVALID_SIZE"};
	// throw BMatException(BMatException::INVALID_SIZE,
	// 		     "BoolMatrix::operator*()");

    BoolMatrix temp(mNRows,mNCols);
    uint32_t i,j,k;
    uint32_t m;
    for (i=0; i<mNRows; i++)
	for (j=0; j<mNCols; j++) {
	    m = i*mNRows;
	    temp.mMat[m+j] = false;
	    for (k=0; k<mNCols; k++)
		temp.mMat[m+j] = temp.mMat[m+j] || (mMat[m+k] && a.mMat[k*mNRows+j]);
	}
    return temp;
}

BoolMatrix &BoolMatrix::operator=(const BoolMatrix &inMat)
{
    if (&inMat == this)
	   return (*this);

    if (mMat != nullptr) {
    	delete [] mMat;
    	mMat = nullptr;
    }

    try {
    	mNRows = inMat.mNRows;
    	mNCols = inMat.mNCols;
    	mMat = new bool[mNRows*mNCols];
    	for (uint32_t i=0; i<mNRows*mNCols; i++)
    	    mMat[i] = inMat.mMat[i];
    } catch (bad_alloc &ba) {
        throw runtime_error{"BAD_ALLOCATION"};
	// throw BMatException(BMatException::BAD_ALLOCATION,
	// 		     "BoolMatrix::BoolMatrix()");
    }

    return (*this);
}

ostream &colibry::operator<<(ostream &os, const BoolMatrix &bm)
{
    for (uint32_t i=0; i<bm.mNRows; i++) {
    	for (uint32_t j=0; j<bm.mNCols; j++)
    	    os << bm.mMat[i*bm.mNRows+j];
    	os << endl;
    }
    return os;
}


// Defining the static member
// Declaring the static member is not enough. We need to DEFINE it in the
// program too.
SymTable Automaton::mIOT;

// +-------------+
// | Constructor |
// +-------------+-----------------------------------------------

Automaton::Automaton()
{
    mInitialState = nullptr;
    mCurrState = mInitialState;
    mIsDeterministic = true;
    mTransCount = mStateCount = 0;
    mStVec.reserve(AUTRESERVED_SIZE);
}

Automaton::Automaton(const string &inFileName)
{
    mInitialState = mCurrState = nullptr;
    mIsDeterministic = true;
    mTransCount = mStateCount = 0;
    ReadFile(inFileName);
}

Automaton::~Automaton()
{
    Clear();
}

Automaton &Automaton::operator=(const Automaton &aut)
{
    if (&aut == this)
    	return *this;

    Clear(); // Clear current automaton...

    mStVec = aut.mStVec;
    mInitialState = (aut.mInitialState==nullptr) ? nullptr : aut.mStVec[aut.mInitialState->get_state_no()];
    mCurrState = (aut.mCurrState==nullptr) ? nullptr : aut.mStVec[aut.mCurrState->get_state_no()];
    mIsDeterministic = aut.mIsDeterministic;
    mTransCount = aut.mTransCount;
    mStateCount = aut.mStateCount;

    return (*this);
}

////
// Add
////
void Automaton::Add(StateType is,
		SigType in,
		SigType out,
		StateType fs)
     // Add transations between states, creating these states, if necessary.
     // The first state created is assumed to be the inicial state.
{
    // Check signal numbers
    if (!mIOT.IsValid(in) || !mIOT.IsValid(out))
        throw runtime_error{"INVALID_SIGN"};
    	// throw AutException(AutException::INVALID_SIGN,"Automaton::Add");

    // Find pointers to initial and final states
    Node *pis, *pfs;
    if (is<mStVec.size())
    	pis = mStVec[is];
    else {
    	// Resize vector
    	mStVec.resize(is+1,nullptr);
    	if (mStVec.capacity() == mStVec.size())   // To avoid moving vector around
    	    mStVec.reserve(mStVec.capacity()+AUTRESERVED_SIZE);
    	pis = nullptr;
    }
    if (pis == nullptr) {
	// Create pis state
	pis = new Node(is);
	mStVec[is] = pis;
	mStateCount++;
	// If first created, becomes initial state
	if (mInitialState == nullptr)
	    mInitialState = mCurrState = pis;
    }

    if (fs < mStVec.size()) {
	pfs = mStVec[fs];
    } else {
	mStVec.resize(fs+1,nullptr);
	if (mStVec.capacity() == mStVec.size())	// To avoid moving vector around
	    mStVec.reserve(mStVec.capacity()+AUTRESERVED_SIZE);
	pfs = nullptr;
    }
    if (pfs == nullptr) {
	// Creat pfs state
	pfs = new Node(fs);
	mStVec[fs] = pfs;
	mStateCount++;
    }

    // pis & pfs are valid pointers to is & fs nodes

    // Add edge between pis & pfs

    Edge tr;
    tr.input = in;
    tr.output = out;
    tr.isVisited = tr.isAbsVisited = false;
    tr.toNode = pfs;
    pis->AddEdge(tr);

    mTransCount++;
}


// +-----+
// | Add |
// +-----+-------------------------------------------------------

void
Automaton::Add(StateType is,
		const string &in,
		const string &out,
		StateType fs)
{
    SigType ins = mIOT.LookUp(in);
    SigType ous = mIOT.LookUp(out);
    Add(is,ins,ous,fs);
}


// +-------+
// | Delta |
// +-------+-----------------------------------------------------

StateType
Automaton::Delta(const string &in, string &out)
    // Execute transition from the current state.
    // ERRORS: EMPTY_AUTOMATON - obvious!
    //         NO_TRANSITION - if no NON-VISITED transition
{
    if (mStVec.empty())
        throw runtime_error{"EMPTY_AUTOMATON"};
	// throw AutException(AutException::EMPTY_AUTOMATON,"Automaton::Delta()");

    list<Edge> &el = *(mCurrState->get_edge_list());	// shorthand
    mCurrState->Mark();

    if (el.empty())
        throw runtime_error{"NO_TRANSITION"};
	// throw (AutException(AutException::NO_TRANSITION,"Automaton::Delta()"));

    // Current state exists and has outgoing transitions

    try {
        SigType insig;
        insig = mIOT.Find(in);
        list<Edge>::iterator atrans;
        for (atrans = el.begin(); atrans != el.end(); atrans++)
        	if (atrans->input == insig)
        	    if (!atrans->isVisited) {
            		atrans->isVisited = true;
            		atrans->isAbsVisited = true;
            		out = mIOT.GetSymbol(atrans->output);
            		mCurrState = atrans->toNode;
            		return mCurrState->get_state_no();
        	    }
        throw runtime_error{"NO_TRANSITION"};
        // throw AutException(AutException::NO_TRANSITION,"Automaton::Delta()");
    } catch(const SymTable::not_found&) {
        throw runtime_error{"NO_TRANSITION"};
        // throw (AutException(AutException::NO_TRANSITION,"Automaton::Delta()"));
    }
}


// +-------+
// | reset |
// +-------+-----------------------------------------------------

void Automaton::Reset()
    // Reset Automaton to the initial state
{
    mCurrState = mInitialState;
    UnvisitAll();
}

// +--------+
// | currst |
// +--------+----------------------------------------------------

StateType Automaton::GetCurrState()
{
    if (mStVec.empty())
        throw runtime_error{"EMPTY_AUTOMATON"};
	// throw(AutException(AutException::EMPTY_AUTOMATON,"Automaton::GetCurrState"));

    return mCurrState->get_state_no();
}

StateType Automaton::GetInitialState()
{
    if (mStVec.empty())
        throw runtime_error{"EMPTY_AUTOMATON"};
	// throw(AutException(AutException::EMPTY_AUTOMATON,"Automaton::GetInitialState"));

    return mInitialState->get_state_no();
}

void Automaton::GetValidInput(string &sig)
{
    #pragma unused(sig)
}

void Automaton::SetCurrState(const StateType st)
{
    if (mStVec.empty())
        throw runtime_error{"EMPTY_AUTOMATON"};
	// throw AutException(AutException::EMPTY_AUTOMATON,"Automaton::SetCurrState");

    if (!IsValidSt(st))
        throw runtime_error{"INVALID_STATEN"};
    	// throw AutException(AutException::INVALID_STATEN,"Automaton::SetCurrState()");

    mCurrState = mStVec[st];
}

// +---- Load State Table ------------------------------------------------
// |
// |   Load State Table from file (Aldebaran format).
// |   Do not consider empty lines or lines starting with '#'.
// |
// +----------------------------------------------------------------------

void Automaton::ReadFile(const string &file_name)
{
    ifstream file(file_name.c_str());
    if (!file.is_open())
        throw runtime_error{"FILE_NOT_FOUND"};
    	// throw FileException(FileException::FILE_NOT_FOUND,
			 // "Automaton::ReadFile()",file_name);

    // Get and parse each line
    Line line;
    StateType is, fs;
    char *aux, *in, *out;
//    uint32_t ln = 1;

    // Get automaton description (1st line)
    file.getline(line,MAXLINELEN);
    in = ::strchr(line,'(');
    throw_if(in == nullptr, "WRONG_FORMAT");
    ++in;
    aux = ::strchr(in,',');
    throw_if(aux == nullptr, "WRONG_FORMAT");
    *aux = '\0';
    StateType initialState = ::strtoul(in, (char**)nullptr, 10);
    // Ignore # of transitions
    // Get # of states
    in = ::strrchr(++aux,',');
    throw_if(in == nullptr, "WRONG_FORMAT");
    ++in;
    aux = ::strchr(in,')');
    throw_if(aux == nullptr, "WRONG_FORMAT");
    *aux = '\0';

    mStVec.reserve(::strtoul(in, (char**)nullptr, 10)+1);		// Expected number of states

    while(file.getline(line,MAXLINELEN)) {
    	// Parse line
    	if (line[0] == '\0' || line[0] == '#' || (::strncmp(line,"//",2)==0)) {
    	    // Discard empty lines instead of throwing exception
    	    // Allow comments (starting with '#' or with '//')
//    	    ln++;
    	    continue;
    	}
    	aux = strchr(line,'(');
        throw_if(aux == nullptr, "WRONG_FORMAT");
    	++aux;
    	in = strchr(line,',');
        throw_if(in == nullptr, "WRONG_FORMAT");
    	*(in++) = '\0';
    	is = strtoul(aux, (char**)nullptr, 10);

    	// Find last comma
    	aux=strrchr(in,',');
        throw_if(aux == nullptr, "WRONG_FORMAT");
    	fs = strtoul(aux+1, (char**)nullptr, 10);
    	*(aux-1) = '\0';
    	++in;
    	aux = strchr(in,'/');
        throw_if(aux == nullptr, "WRONG_FORMAT");
    	out = aux+1;
    	*aux = '\0';

    	Add(is,in,out,fs);
//    	ln++;
    }

    mInitialState = mCurrState = mStVec[initialState];

    file.close();
}

////
//  Clear
////
void
Automaton::Clear()
{
    for (StateType i=0; i<mStVec.size(); i++)
	if (mStVec[i] != nullptr)
	    delete mStVec[i];

    mStVec.clear();
    mInitialState = mCurrState = nullptr;
    mIsDeterministic = true;
    mTransCount = mStateCount = 0;
}

void Automaton::Unvisit(StateType i)
{
    throw_if(mStVec.empty(), "EMPTY_AUTOMATON");
    // if (mStVec.empty())
	// throw AutException(AutException::EMPTY_AUTOMATON,"Automaton::Unvisit()");

    throw_if(IsValidSt(i), "INVALID_STATEN");
    // if (!IsValidSt(i))
	// throw AutException(AutException::INVALID_STATEN,"Automaton::Unvisit()");

    Node *theNd = mStVec[i];

    // Unvisit state
    theNd->Mark(false);

    // Unvisit outgoing transitions
    list<Edge>::iterator eit;
    list<Edge> *the_list = theNd->get_edge_list();
    for (eit=the_list->begin(); eit!=the_list->end(); eit++)
    	eit->isVisited = false;
}


// Unvisitall
void Automaton::UnvisitAll()
{
    if (mStVec.empty())
    	return;

    Node *anode;
    list<Edge>::iterator aedge;
    list<Edge>* the_list;

    for (StateType i=0; i!=mStVec.size(); i++)
	if ((anode=mStVec[i]) != nullptr) {
	    anode->Mark(false);
	    the_list = anode->get_edge_list();
	    for (aedge=the_list->begin(); aedge!=the_list->end(); aedge++)
		aedge->isVisited = false;
	}
}


// +----------+
// | Removetr |
// +----------+------------------------------------------------

void
Automaton::RemoveTr(StateType is,
		     const string &in,
		     const string &out,
		     StateType fs)
{
    throw_if(mStVec.empty(), "EMPTY_AUTOMATON");

    // if (mStVec.empty())
	//    throw AutException(AutException::EMPTY_AUTOMATON,"Automaton::RemoveTr");

    throw_if(!IsValidSt(is) || !IsValidSt(fs), "NO_TRANSITION");
    // if (!IsValidSt(is) || !IsValidSt(fs))
	//    throw AutException(AutException::NO_TRANSITION,"Automaton::RemoveTr()");

    Node *pis, *pfs;
    pis = mStVec[is];
    pfs = mStVec[fs];

    try {
        Edge sked;
        sked.fromNode = pis;
        sked.toNode = pfs;
        sked.input = mIOT.Find(in);
        sked.output = mIOT.Find(out);

        throw_if(!pis->RemoveEdge(sked), "NO_TRANSITION");
        // if (!pis->RemoveEdge(sked))
        // 	throw (AutException(AutException::NO_TRANSITION,"Automaton::RemoveTr"));
        --mTransCount;

        // If there are no outgoing transitions from 'is' AND no incoming transitions
        // to 'is', it can be deleted.
        if (pis->get_edge_list()->empty() && pis->get_indegree()==0) {
        	mStVec[is] = nullptr;
        	delete pis;
        	mStateCount--;
        }
    } catch (const SymTable::not_found&) {
        throw runtime_error{"NO_TRANSITION"};
        // throw AutException(AutException::NO_TRANSITION,"Automaton::RemoveTr()");
    }
}


bool Automaton::HasAbsUnvisited()
{
    Node *anode;
    list<Edge>::iterator aedge;
    list<Edge>* the_list;

    for (StateType i=0; i<mStVec.size(); i++)
	if ((anode=mStVec[i]) != nullptr) {
	    the_list = anode->get_edge_list();
	    for (aedge=the_list->begin(); aedge!=the_list->end(); aedge++)
		if (!aedge->isAbsVisited)
		    return true;
	}

    return false;
}


void
Automaton::DisplayAbsUnvisited(ostream &os, const string &beforeEach)
{
    Node *anode;
    list<Edge>::iterator aedge;
    list<Edge>* el;

    for (StateType i=0; i<mStVec.size(); i++)
	if ((anode=mStVec[i]) != nullptr) {
	    if (!anode->IsMarked())
		os << beforeEach << "State: " << anode->get_state_no() << endl;
	    el = anode->get_edge_list();
	    for (aedge=el->begin(); aedge!=el->end(); aedge++)
		if (!aedge->isAbsVisited)
		    os << beforeEach
		       << "(" << *anode << ",\""
		       << mIOT.GetSymbol(aedge->input) << "/"
		       << mIOT.GetSymbol(aedge->output) << "\","
		       << *(aedge->toNode) << ")" << endl;
	}
}

bool
Automaton::IsDeterministic() const
{
    return mIsDeterministic;
}

bool Automaton::IsValidSt(StateType st)
{
    if (st<mStVec.size())
    	return (mStVec[st]!=nullptr);

    return false;
}

uint32_t Automaton::GetStateCount() const
{
    return mStateCount;
}

uint32_t Automaton::GetTransCount() const
{
    return mTransCount;
}

#pragma mark -
#pragma mark === TCLOSURE METHODS


unique_ptr<BoolMatrix> Automaton::GenerateAdjMatrix()
{
    throw_if(mStVec.size() != mStateCount, "HAS_HOLES");

    // if (mStVec.size() != mStateCount)
	// throw AutException(AutException::HAS_HOLES,"Automaton::GenerateAdjMatrix()");

    const uint32_t N = GetStateCount();
    if (N==0) return nullptr;

    auto theAdj = make_unique<BoolMatrix>(N,N);

    // Initialize matrix
    for (StateType i=0; i<N; i++)
    	for (StateType j=0; j<N; j++)
    	    (*theAdj)(i,j) = false;

    // Node *anode = nullptr;
    list<Edge>::iterator aedge;
    list<Edge>* el = nullptr;

    // mStVec doesn't contain holes
    for (auto* anode : mStVec) {
        el = anode->get_edge_list();
        for (aedge=el->begin();aedge!=el->end();aedge++)
            (*theAdj)(anode->get_state_no(), aedge->toNode->get_state_no()) = true;
    }

    // for (StateType i=0; i<mStVec.size(); i++) {
	// anode = mStVec[i];
	// el = anode->get_edge_list();
	// for (aedge=el->begin();aedge!=el->end();aedge++)
	//     (*theAdj)(anode->get_state_no(), aedge->toNode->get_state_no()) = true;
    // }

    return theAdj;
}

unique_ptr<BoolMatrix> Automaton::GenerateTClosure()
{
    const uint32_t N = GetStateCount();
    auto adj = GenerateAdjMatrix();
    auto adjka = make_unique<BoolMatrix>(*adj);     // adj k-1
    // BoolMatrix *adjka = new BoolMatrix(*adj);		// adj k-1
    auto adjk = make_unique<BoolMatrix>(N,N);       // adj k
    // BoolMatrix *adjk = new BoolMatrix(N,N);		// adj k
    auto tc = make_unique<BoolMatrix>(*adj);
    // BoolMatrix *tc = new BoolMatrix(*adj);
    unique_ptr<BoolMatrix> aux;
    for (StateType i=2; i<N; i++) {
    	*adjk = (*adj) * (*adjka);
    	*tc |= *adjk;
    	aux = std::move(adjk);
    	adjk = std::move(adjka);
    	adjka = std::move(aux);
    }

    // delete adj;
    // delete adjka;
    // delete adjk;

    return tc;
}


list<list<StateType>> *Automaton::GetConnectedComponents()
{
    const uint32_t N = GetStateCount();
    list<StateType> skip;
    list<StateType> ccomp;
    list< list<StateType> > *theList = new list< list<StateType> >;
    auto tc = GenerateTClosure();

    for (uint32_t i=0; i<N; i++) {

    	// Check for path from i to i
    	if (!(*tc)(i,i))
    	    continue;

    	if (find(skip.begin(), skip.end(), i) != skip.end())
    	    continue;

    	ccomp.clear();
    	ccomp.push_back(i);
    	for (uint32_t j=0; j<i; j++) {	// Lines
    	    if (find(skip.begin(), skip.end(), j) != skip.end())
    		continue;
    	    if ((*tc)(i,j)) {
    				// Compare line i with line j
    		if (CompareLines(&*tc,i,j))
    		    ccomp.push_back(j);
    	    }
    	}
    	for (uint32_t j=i+1; j<N; j++) {
    	    if ((*tc)(i,j)) {
    		if (CompareLines(&*tc,i,j))
    		    ccomp.push_back(j);
    	    }
    	}

    	// Append ccomp to skip
    	copy(ccomp.begin(),ccomp.end(),back_inserter(skip));

    	theList->push_back(ccomp);
    }
    return theList;
}


// operator<<

ostream& colibry::operator<<(ostream& os, colibry::Automaton &a)
{
    if (a.mStVec.empty())
	return (os << "des(0,0,0)");

    // Aldebaran (TM) header
    os << "des(" << a.GetInitialState() << "," << a.GetTransCount()
       << "," << a.GetStateCount() << ")" << endl;

    Node *anode;
    list<Edge>::iterator aedge;
    list<Edge>* el;

    for (StateType i=0; i<a.mStVec.size(); i++)
	if ((anode=a.mStVec[i]) != nullptr) {
	    el = anode->get_edge_list();
	    for (aedge=el->begin(); aedge!=el->end(); aedge++)
		os << "(" << *anode << ",\""
		   << Automaton::mIOT.GetSymbol(aedge->input) << "/"
		   << Automaton::mIOT.GetSymbol(aedge->output) << "\","
		   << *(aedge->toNode) << ")\n";
	}

    return os;
}

#pragma mark -
#pragma mark === EXCEPTION

// -----------------------------------------------------------------------
// AUTOMATON EXCEPTION
// -----------------------------------------------------------------------

/*
AutException::AutException(int type, const string& where)
    : Exception((int)type,where)
{
    switch (mType) {
    case INVALID_STATEN:
	mWhat = "Invalid state number";
	break;
    case INVALID_SIGN:
	mWhat = "Invalid signal number";
	break;
    case NO_TRANSITION:
	mWhat = "No transition with given input";
	break;
    case NO_UNVISITED_TRANSITION:
	mWhat = "No unvisited transition with given input";
	break;
    case EMPTY_AUTOMATON:
	mWhat = "Empty automaton";
	break;
    case HAS_ANOTHER_TRANS:
	mWhat = "Delta function chose between two or more transitions";
	break;
    case DUPLICATE_TRANS:
	mWhat = "Tried to insert duplicate transition";
	break;
    case HAS_HOLES:
	mWhat = "State table has holes. Computing adjacency matrix is impossible.";
	break;
    default:
	mWhat = "Unknown exception.";
    }
}
*/

#pragma mark -
#pragma mark === USEFUL FUNCTIONS

bool CompareLines(const BoolMatrix *bm, uint32_t ln1, uint32_t ln2)
{
    const uint32_t N = bm->GetNRows();
    for (uint32_t i=0; i<N; i++)
	if ((*bm)(ln1,i) != (*bm)(ln2,i))
	    return false;
    return true;
}
