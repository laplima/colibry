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
#include <list>
#include <iostream>
#include <stdexcept>
#include <cinttypes>	// supposed T types
#include <limits>
#include <random>
#include <algorithm>
#include <cassert>

namespace colibry {

	// Bag
	template<typename T>
	class Bag {
	public:

		Bag();				// use min and max values for type T in limits
		Bag(T lo);	// specifies only lower bound value
		Bag(T lo, T up);
		Bag(const Bag<T>& bg);
		Bag<T>& operator=(const Bag<T>& bg);

		T get();
		void put_back(T i);

        void reset();   // restore original range

		bool empty() const;
		void randomize(bool r=true);

		T lower() const { return range_.lower; }
		T upper() const { return range_.upper; }

		friend std::ostream& operator<<(std::ostream& os, const Bag<T>& b)
		{
			// Format: [l,u]s[l0,u0][l1,u1]...[l(s-1),u(s-1)]
			os << b.range_ << b.available_.size();
			for (auto& inter : b.available_)
				os << inter;
			return os;
		}

		friend std::istream& operator>>(std::istream& is, Bag<T>& b)
		{
			try {
				decltype(available_.size()) s;
				if (!(is >> b.range_ >> s)) throw std::runtime_error("range");
				Range r;
				b.available_.clear();
				for (decltype(s) i = 0; i<s && is>>r; ++i)
					b.available_.push_back(r);
			} catch (const std::runtime_error& e) {
				is.clear(std::ios::failbit);
			}
			return is;
		}

	protected:

		struct Range {			// continuous range [lo-up]
			T lower;
			T upper;
			Range() {}
			Range(T lo, T up) : lower{lo}, upper{up} {}
			bool empty() const { return (lower > upper); }
			bool contains(T i) const { return (i >= lower && i <= upper); }
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

		Range range_;					// range to take items from
		std::list<Range> available_;	// ranges of available items
		bool randomize_;

		// random static
		static std::random_device rd;
		static std::default_random_engine gen;
	};

	// random static
	template<typename T> std::random_device Bag<T>::rd;
	template<typename T> std::default_random_engine Bag<T>::gen{rd()};

	//
	// Constructors
	//

	template<typename T>
	Bag<T>::Bag()
		: range_{std::numeric_limits<T>::min(), std::numeric_limits<T>::max()},
		randomize_{false}
	{
		available_.emplace_back(range_);
	}

	template<typename T>
	Bag<T>::Bag(T lo)
		: range_{lo, std::numeric_limits<T>::max()},
		randomize_{false}
	{
		available_.emplace_back(range_);
	}

	template<typename T>
	Bag<T>::Bag(T lo, T up)
		: range_{lo, up},
		randomize_{false}
	{
		if (range_.empty())
			throw std::out_of_range{"empty bag"};
		available_.emplace_back(range_);
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
			range_ = bg.range_;
			available_ = bg.available_;
			randomize_ = bg.randomize_;
		}
		return *this;
	}

    template<typename T>
    void Bag<T>::reset()
    {
        available_.clear();
        available_.push_back(range_);
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
		auto it = available_.begin();

		if (!randomize_) {
			// sequential
			id = it->lower;
			++(it->lower);
			if (it->empty())
				available_.pop_front();
		} else {
			// random
			std::uniform_int_distribution<decltype(available_.size())> ud(0,available_.size()-1);
			it = std::next(it,ud(gen));

			// random value in the Range
			std::uniform_int_distribution<T> ut(it->lower,it->upper);
			id = ut(gen);
			// split it range
			Range previnter{it->lower,id-1};	// before it
			it->lower = id+1;
			if (!previnter.empty())
				available_.insert(it,previnter); // insert before it
			if (it->empty())
				available_.erase(it);
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
		if (!range_.contains(i))
			throw std::out_of_range{"put_back(): out of range"};

		if (empty())
			available_.push_back(Range{i,i});
		else {
			// Find position (first range with lower>i)
			auto it = std::find_if(std::begin(available_), std::end(available_), [i](const Range& r) {
				return (r.lower > i);
			});
			// cases
			if (it == std::begin(available_)) {
				// before the first range
				if (it->lower == i+1)
					--(it->lower);
				else
					available_.insert(it, Range{i,i});	// at the beginning
			} else if (it == std::end(available_)) {
				// after the last range
				if (available_.back().upper >= i)
					throw std::invalid_argument{"put_back(): already in bag"};
				else if (available_.back().upper == i-1)
					++(available_.back().upper);
				else
					available_.push_back(Range{i,i}); 	// at the end
			} else {
				// in the middle
				auto ant = std::prev(it);
				auto try_merge = [this](typename std::list<Range>::iterator a, typename std::list<Range>::iterator i) {
					if (a->upper == i->lower - 1) {
						a->upper = i->upper;
						this->available_.erase(i);
					}
				};
				if (ant->upper >= i)
					throw std::invalid_argument{"put_back(): already in bag"};
				else if (it->lower == i+1) {
					--(it->lower);
					try_merge(ant,it);
				} else if (ant->upper == i-1) {
					++(ant->upper);
					try_merge(ant,it);
				} else
					available_.insert(it,Range{i,i});
			}
		}
	}

	//
	// Helpers
	//

	template<typename T>
	bool Bag<T>::empty() const
	{
		return (bool)(available_.empty());
	}

	template<typename T>
	void Bag<T>::randomize(bool r)
	{
		randomize_ = r;
	}

};

#endif
