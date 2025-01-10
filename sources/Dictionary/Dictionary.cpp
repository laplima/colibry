#include "Dictionary.h"
#include <stdexcept>
#include <algorithm>

// debug
#include <iostream>
using std::cout;

constexpr uint64_t ALLOCATION_SZ = 4096;

using namespace colibry;

Dictionary::Dictionary()
{
	symbols_.reserve(ALLOCATION_SZ);	// doesn't affect vector size
}

Dictionary::Dictionary(Dictionary&& d) noexcept
	: maps2i_{std::move(d.maps2i_)},
	symbols_{std::move(d.symbols_)},
	bag_{std::move(d.bag_)}
{
}

Dictionary::~Dictionary()
{
	clear();
}

Dictionary& Dictionary::operator=(Dictionary d) noexcept
{
	::swap(d, *this);
	return *this;
}

void swap(colibry::Dictionary& d1, colibry::Dictionary& d2) noexcept
{
	d1.swap(d2);
}

void Dictionary::swap(Dictionary& d) noexcept
{
	using std::swap;
	swap(d.maps2i_, maps2i_);
	swap(d.symbols_, symbols_);
	swap(d.bag_, bag_);
}

void Dictionary::check_index(uint64_t index) const
{
	if (index >= symbols_.size() || symbols_[index].symbol == nullptr)
		throw std::out_of_range{"Invalid index"}; 
}

uint64_t Dictionary::operator[](const std::string& symbol)
{
	// throw out-of-range if not found
	return maps2i_.at(symbol);
}

const std::string& Dictionary::operator[](const uint64_t index)
{
	check_index(index);
	return *(symbols_[index].symbol);
}

uint64_t Dictionary::lookup(const std::string& symbol)
{
	auto p = maps2i_.find(symbol);
	if (p == maps2i_.end()) {
		// not found -- create
		auto i = bag_.get();
		auto [it,_] = maps2i_.emplace(symbol,i);	// iterator
		// make sure index exist in the vector
		// (may cause memory relocation!)
		if (symbols_.size() <= i) {
			symbols_.resize(i+1);
			if (i%ALLOCATION_SZ == 0 && i != 0)
				symbols_.reserve(i+ALLOCATION_SZ);
		}
		symbols_[i] = Item{&(it->first)};
		return i;
	} else
		return p->second;
}

void Dictionary::remove(const uint64_t index)
{
	check_index(index);
	std::string symbol = *symbols_[index].symbol;
	// todo: change symbols_'s size...
	symbols_[index].symbol = nullptr;
	maps2i_.erase(symbol);
	bag_.put_back(index);
}

void Dictionary::remove(const std::string &symbol)
{
	auto index = maps2i_.at(symbol);	// throw if not found
	symbols_[index].symbol = nullptr;
	maps2i_.erase(symbol);
	bag_.put_back(index);
}

void Dictionary::clear() noexcept
{
	symbols_.clear();
	maps2i_.clear();
	bag_.reset();
}

void Dictionary::mark(const uint64_t index, bool marked)
{
	check_index(index);
	symbols_[index].marked = marked;
}

bool Dictionary::is_marked(const uint64_t index) const
{
	check_index(index);
	return symbols_[index].marked;
}
