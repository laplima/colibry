//
// BAG
//
// Copyright (C) 2006 by LAPLJ.
// Allrights reserved.
//

#ifndef __BAG_H__
#define __BAG_H__

#include <list>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <colibry/Nullable.h>

namespace colibry {

    // Exceptions
    class EmptyBagException {};
    class AlreadyInBagException {};
    class OutOfBoundsException {};

    template<class T>
    class Bag {
    public:
        Bag() : m_isbounded(false) {}  // unbounded bag
        Bag(const T lo, const T up);   // bounded bag
        Bag(const Bag<T>& bg);

        Bag<T>& operator=(const Bag<T>& bg);

        T Get() throw (EmptyBagException);
        void PutBack(const T i)
            throw (AlreadyInBagException,OutOfBoundsException);

        bool IsEmpty();
        void Randomize(const bool r=true);

        void Show();

    protected:

	struct Interval {
	    Interval() {}
	    Interval(const T lo, const T up) { lower = lo; upper = up; }
	    T lower;
	    T upper;
	};
	Nullable<Interval> m_limits;
	std::list<Interval> m_data;
        bool m_randomize;
	bool m_isbounded;
    };

    //
    // Constructors
    //

    template<class T>
    Bag<T>::Bag(const T lo, const T up)
    {
	m_limits = Interval(lo,up);
	m_isbounded = true;
	m_data.push_back(m_limits.get());
	m_randomize = false;
    }

    template<class T>
    Bag<T>::Bag(const Bag<T>& bg)
    {
	*this = bg;
    }

    //
    // Assignment
    //

    template<class T>
    Bag<T>& Bag<T>::operator=(const Bag<T>& bg)
    {
	if (this != &bg) {
	    m_limits = bg.m_limits;
	    m_data = bg.m_data;
	    m_randomize = bg.m_randomize;
	    m_isbounded = bg.m_isbounded;
	}
	return *this;
    }

    //
    // Get
    //

    template<class T>
    T Bag<T>::Get() throw (EmptyBagException)
    {
	if (IsEmpty())
	    throw EmptyBagException();

	T id;
	typename std::list<Interval>::iterator it;
	it = m_data.begin();

	if (!m_randomize) {
	    it->lower++;
	    if (it->lower > it->upper)
		m_data.pop_front();		// remove interval
	    id = it->lower - 1;
	} else {

	    // randomize
	    unsigned long x = random()%m_data.size();
	    for (unsigned long i=0; i<x; i++)
		it++;

	    x = random()%(it->upper - it->lower + 1);
	    id = it->lower + x;
	    if (id == it->lower)
		it->lower++;
	    else if (id == it->upper)
		it->upper--;
	    else {
		m_data.insert(it,Interval(it->lower,id-1));
		it->lower = id+1;
	    }
	    if (it->lower > it->upper)
		m_data.erase(it);
	}

	return id;
    }

    //
    // PutBack
    //

    template<class T>
    void Bag<T>::PutBack(const T i)
	throw (AlreadyInBagException,OutOfBoundsException)
    {
	if (m_isbounded) {
	    if (i>m_limits.get().upper || i<m_limits.get().lower)
		throw OutOfBoundsException();
	}

	if (IsEmpty()) {
	    m_data.push_back(Interval(i,i));
	    return;
	}

	// Find position
	typename std::list<Interval>::iterator it, ant;
	it = ant = m_data.begin();
	while (it->lower <= i && it!=m_data.end()) {
	    ant = it;
	    it++;
	}

	if (ant->upper >= i && ant != it) {
	    // already in bag
	    throw AlreadyInBagException();
	}

	// update limits for unbounded bags
	if (!m_isbounded) {
	    if (m_limits.IsNull())
		m_limits = Interval(i,i);
	    else {
		if (i < m_limits.get().lower)
		    m_limits.ptr()->lower = i;
		if (i > m_limits.get().upper)
		    m_limits.ptr()->upper = i;
	    }
	}

	if (it == m_data.end()) {
	    // i is after last interval
	    it--;		// back to last
	    if (it->upper == i-1)
		it->upper++;     // add to last interval
	    else {
		// Create new last interval
		m_data.push_back(Interval(i,i));
	    }
	} else if (it == ant) {
	    // i is before first
	    if (it->lower == i+1)
		(it->lower)--;
	    else
		m_data.insert(it,Interval(i,i));
	} else {
	    // i is in middle
	    if (it->lower == i+1)
		(it->lower)--;   // add to interval
	    else if (ant->upper == i-1)
		ant->upper++;
	    else {
		// Create new interval
		m_data.insert(it,Interval(i,i));
	    }
	    if (it->lower == ant->upper+1) {
		// Concatenate
		ant->upper = it->upper;
		m_data.erase(it);
	    }
	}
    }

    //
    // Helpers
    //

    template<class T>
    bool Bag<T>::IsEmpty()
    {
	return (bool)(m_data.empty());
    }

    template<class T>
    void Bag<T>::Randomize(const bool r)
    {
	m_randomize = r;
	if (r)
	    srand(time(NULL));
    }

    template<class T>
    void Bag<T>::Show()
    {
	typename std::list<Interval>::iterator it;
	for (it=m_data.begin(); it!=m_data.end(); it++) {
	    std::cout << "[" << it->lower << "," << it->upper << "] ";
	}
	std::cout << std::endl;
    }

};	// end namespace

#endif
