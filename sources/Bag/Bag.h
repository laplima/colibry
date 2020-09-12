//
// "NEW" BAG
//
// Copyright (C) 2006-2020 by LAPLJ.
// All rights reserved.
//
// Bag elements are incremented by 1.
//
// Exceptions that may thrown:
//
// constructor: logic_error::out_of_range  => creating empty bag
// get():       runtime_error::underflow_error  => no more items in the bag
// put_back():  logic_error::invalid_argument => already in bag
//              logic_error::out_of_range => item doesn't belong to the bag
//

#ifndef __BAG_H__
#define __BAG_H__

#include <vector>
#include <iostream>
#include <stdexcept>
#include <cinttypes>	// supposed T types
#include <limits>
#include <random>

namespace colibry {

	// Bag
	template<typename T>
	class Bag {
	public:

		Bag();				// use min and max values for type T in limits
		Bag(const T lo);	// specifies only lower bound value
		Bag(const T lo, const T up);
		Bag(const Bag<T>& bg);
		Bag<T>& operator=(const Bag<T>& bg);

		T get();
		void put_back(const T i);

		bool empty() const;
		void randomize(bool r=true);

		T lower() const { return m_range.lower; }
		T upper() const { return m_range.upper; }

		friend std::ostream& operator<<(std::ostream& os, const Bag<T>& b)
		{
			// Format: [l,u]s[l0,u0][l1,u1]...[l(s-1),u(s-1)]
			os << b.m_range << b.m_available.size();
			for (auto& inter : b.m_available)
				os << inter;
			return os;
		}

		friend std::istream& operator>>(std::istream& is, Bag<T>& b)
		{
			try {
				decltype(m_available.size()) s;
				if (!(is >> b.m_range >> s)) throw std::runtime_error("range");
				Range r;
				b.m_available.clear();
				for (decltype(s) i = 0; i<s && is>>r; ++i)
					b.m_available.push_back(r);
			} catch (const std::runtime_error& e) {
				is.clear(std::ios::failbit);
			}
			return is;
		}

	protected:

		struct Range {
			Range() {}
			Range(const T lo, const T up) : lower{lo}, upper{up} {}
			bool empty() const { return (lower > upper); }
			T lower;
			T upper;
			friend std::ostream& operator<<(std::ostream& os, const Range& r) {
				return (os << "[" << r.lower << "," << r.upper << "]");
			}
			friend std::istream& operator>>(std::istream& is, Range& r) {
				try {
					char c;
					if (!(is >> c) || c != '[') throw std::runtime_error{"["};
					if (!(is >> r.lower >> c) || c != ',') throw std::runtime_error{","};
					if (!(is >> r.upper >> c) || c != ']') throw std::runtime_error{"["};
				} catch (std::runtime_error& e) {
					is.clear(std::ios::failbit);
				}
				return is;
			}

		};

	protected:

		Range m_range;					// range to take items from
		std::vector<Range> m_available;	// ranges of available items
		bool m_randomize;
	};

	//
	// Constructors
	//

	template<typename T>
	Bag<T>::Bag()
		: m_range{std::numeric_limits<T>::min(), std::numeric_limits<T>::max()},
		m_randomize{false}
	{
		m_available.push_back(m_range);
	}

	template<typename T>
	Bag<T>::Bag(const T lo)
		: m_range{lo, std::numeric_limits<T>::max()},
		m_randomize{false}
	{
		m_available.push_back(m_range);
	}

	template<typename T>
	Bag<T>::Bag(const T lo, const T up)
		: m_range{lo, up},
		m_randomize{false}
	{
		if (m_range.empty()) throw std::out_of_range{"empty bag"};
		m_available.push_back(m_range);
	}

	//
	// Copy constructor
	//

	template<typename T>
	Bag<T>::Bag(const Bag<T>& bg)
	{
		*this = bg;
	}

	//
	// Assignment
	//

	template<typename T>
	Bag<T>& Bag<T>::operator=(const Bag<T>& bg)
	{
		if (this != &bg) {
			m_range = bg.m_range;
			m_available = bg.m_available;
			m_randomize = bg.m_randomize;
		}
		return *this;
	}

	//
	// get
	//

	template<typename T>
	T Bag<T>::get()
	{
		if (empty())
			throw std::underflow_error{"no more items"};

		T id;
		auto it = m_available.begin();

		if (!m_randomize) {
			// sequential
			id = it->lower;
			it->lower++;
			if (it->empty())
				m_available.erase(m_available.begin());		// remove range from availables
		} else {
			// random
			// random available range
			std::default_random_engine gen{(std::random_device())()};
			std::uniform_int_distribution<decltype(m_available.size())> ud(0,m_available.size()-1);
			it += ud(gen);

			// random value in the Range
			std::uniform_int_distribution<T> ut(it->lower,it->upper);
			id = ut(gen);
			// split Range
			Range previnter{it->lower,id-1};	// before it
			it->lower = id+1;
			if (!previnter.empty()) {
				it = m_available.insert(it,previnter);
				++it;
			}
			if (it->empty())
				m_available.erase(it);
		}
		return id;
	}

	template<typename T>
	Bag<T>& operator>>(Bag<T>& b, T& x)
	{
		x = b.get();
		return b;
	}

	template<typename T>
	Bag<T>& operator<<(Bag<T>& b, const T& x)
	{
		b.put_back(x);
		return b;
	}

	//
	// PutBack
	//

	template<typename T>
	void Bag<T>::put_back(const T i)
	{
		if (i > m_range.upper || i < m_range.lower) {
			// out of range
			throw std::out_of_range{"put_back(): out of range"};
		} else if (empty()) {
			// bag is empty
			m_available.push_back(Range{i,i});
		} else {
			// Find position (first range with lower>i)
			auto it = m_available.begin();
			while (it->lower<=i && it!=m_available.end())
				++it;
			if (it == m_available.begin()) {
				// before first range
				if (it->lower == i+1)
					--(it->lower);
				else
					m_available.insert(it, Range{i,i});	// at the beginning
			} else if (it == m_available.end()) {
				// after last
				--it;
				if (it->upper >= i)
					throw std::invalid_argument{"put_back(): already in bag"};
				else if (it->upper == i-1)
					++(it->upper);
				else
					m_available.push_back(Range{i,i}); 	// at the end
			} else {
				// in the middle
				auto ant = it-1;
				if (ant->upper >= i)
					throw std::invalid_argument{"put_back(): already in bag"};
				else if (it->lower == i+1) {
					--(it->lower);
					if (ant->upper == it->lower - 1) {
						// concatenate ranges
						it->lower = ant->lower;
						m_available.erase(ant);
					}
				} else if (ant->upper == i-1) {
					++(it->upper);
					if (ant->upper == it->lower - 1) {
						// concatenate ranges
						it->lower = ant->lower;
						m_available.erase(ant);
					}
				} else
					m_available.insert(it,Range{i,i});
			}
		}
	}

	//
	// Helpers
	//

	template<typename T>
	bool Bag<T>::empty() const
	{
		return (bool)(m_available.empty());
	}

	template<typename T>
	void Bag<T>::randomize(bool r)
	{
		m_randomize = r;
	}

};

#endif
