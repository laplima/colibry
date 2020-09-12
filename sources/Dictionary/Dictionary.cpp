#include "Dictionary.h"
#include <exception>
#include <stdexcept>
#include <algorithm>

const uint64_t ALLOCATION_SZ = 4096;

using namespace std;
using namespace colibry;

Dictionary::Dictionary() : m_count{0}
{
	m_symbols.reserve(ALLOCATION_SZ);	// doesn't affect vector size
}

Dictionary::~Dictionary()
{
	Clear();
}

uint64_t Dictionary::operator[](const std::string& symbol)
{
	auto p = m_s2i.find(symbol);
	if (p == m_s2i.end())
		throw invalid_argument{"Symbol not found"};
	return p->second;
}

const std::string& Dictionary::operator[](const uint64_t index)
{
	if (index >= m_symbols.size())
		throw invalid_argument{"Invalid index"};
	return m_symbols[index].symbol;
}

uint64_t Dictionary::LookUp(const std::string& symbol)
{
	try {
		return (*this)[symbol];
	} catch(const invalid_argument&) {
		// not found -- create
		m_s2i[symbol] = m_count;
		m_symbols.push_back(Item{m_s2i.find(symbol)->first});
		return m_count++;
	}
}

void Dictionary::Remove(const uint64_t index)
{
	try {
		string symbol = (*this)[index];
		m_s2i.erase(symbol);	// just remove the map entry
	} catch (const invalid_argument&) {
		throw std::out_of_range{"Remove(index)"};
	}
}

void Dictionary::Remove(const std::string &symbol)
{
	auto n = m_s2i.erase(symbol);
	if (n==0)
		throw std::invalid_argument{"Remove(symbol)"};
}

void Dictionary::Clear() noexcept
{
	m_symbols.clear();
	m_s2i.clear();
}

void Dictionary::Mark(const uint64_t index, bool marked)
{
	// note: mark on vetor (it may not exist, if removed)
	if (index >= m_symbols.size())
		throw std::out_of_range{"Dictionary::Mark()"};
	m_symbols[index].marked = marked;
}

bool Dictionary::IsMarked(const uint64_t index) const
{
	if (index >= m_symbols.size())
		throw std::out_of_range{"Dictionary::IsMarked()"};
	return m_symbols[index].marked;
}
