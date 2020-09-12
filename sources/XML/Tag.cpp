/*
 *  Tag.cpp
 *
 *  Created by Luiz Lima Jr. on Fri Mar 08 2002.
 *  Copyright (c) 2003 LAPLJ. All rights reserved.
 *
 */

#include <vector>
#include <sstream>
#include "Tag.h"
#include "TagList.h"

using namespace std;
using namespace colibry;


// CONSTRUCTORS

Tag::Tag(const string& tag) : m_name(tag)
{
    m_data = NULL;
    m_subtags = new TagList;
    m_type = SELF_CLOSED;
}

Tag::Tag(const string& tag, const string& data) : m_name(tag)
{
    m_data = new string(data);
    m_subtags = new TagList;
    m_type = SIMPLE;
}

// DESTRCUTOR

Tag::~Tag()
{
    if (m_data != NULL)
	delete m_data;
    if (m_subtags != NULL)
	delete m_subtags;
}

// MODIFIERS

void Tag::AddSubtag(Tag* child)
{
    // Destrcutor will destroy child memory...
    m_subtags->push_front(child);
    m_type = NESTED;
}

void Tag::AddParameter(const string& par, const string& val)
{
    m_par_map[par] = val;
}

void Tag::SetData(const string& data)
{
    if (m_data != NULL)
	*m_data = data;
    else
	m_data = new string(data);
    m_type = SIMPLE;
}

void Tag::SetTagName(const string& name)
{
    m_name = name;
}

// SELECTORS
TagType Tag::Type()
{
    if (!m_subtags->empty())
	m_type = NESTED;
    return m_type;
}

string Tag::GetData()
{
    string s;
    if (m_data != NULL)
	s = *m_data;
    return s;
}

string Tag::GetParameterValue(const string& par)
{
    string ret;
    ParameterMap::iterator i;
    i = m_par_map.find(par);
    if (i != m_par_map.end()) {
	// found
	ret = i->second;
    }
    return ret;
}


string Tag::GetOpeningStr()
{
    return MakeOpeningTag(m_name);
}

string Tag::GetClosingStr()
{
    return MakeClosingTag(m_name);
}

// HELPERS

void nspaces(ostream& os, int n)
{
    for (int i=0; i<n; i++)
	os << " ";
}

ostream& PrintTagTree(ostream& os, unsigned int level, colibry::Tag& t)
{
    const int tabsize = 4;
    
    // Opening tag
    nspaces(os,level*tabsize);
    os << "<" << t.GetName();
    
    // Parameters
    for (ParameterMap::iterator it = (t.GetParamMap())->begin();
	 it != (t.GetParamMap())->end(); it++)
	os << " " << it->first << "=\"" << it->second << "\"";
    
    // Data (if any)
    if (t.HasData())
	return (os << ">" << t.GetData() << t.GetClosingStr());
    
    // Doesn't have data, therefore may have subtags
    if ((t.GetSubtagList())->empty()) {
	if (t.GetName()[0] == '?')
	    return (os << " ?>");
	else
	    return (os << " />");
    }
    
    os << ">" << endl;
      
    // Recursively print sub tag list
    list<Tag*>::iterator ti;
    for (ti = (t.GetSubtagList())->begin(); ti != (t.GetSubtagList())->end(); ti++) {
	PrintTagTree(os, level+1, *(*ti));
	os << "\n";
    }
    
    // Closing tag
    nspaces(os,level*tabsize);
    os << t.GetClosingStr();

    return os;
}

ostream &colibry::operator<<(ostream &os, colibry::Tag &t)
{
    return PrintTagTree(os,0,t);
}

string Tag::MakeOpeningTag(const string &t, ParameterMap* pmap)
{
    ostringstream otag;
    
    otag << "<" << t;
    // Parameters
    if (pmap != NULL)
	for (ParameterMap::iterator it = pmap->begin(); it != pmap->end(); it++)
	    otag << " " << it->first << "=\"" << it->second << "\"";
    otag << ">";
    return otag.str();
}


string Tag::MakeClosingTag(const string& t)
{
    string ctag("</");
    ctag += t + ">";
    return ctag;
}

