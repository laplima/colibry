/*
 *  TagList.cpp
 *
 *
 *  Created by Luiz Lima Jr. on Tue Jul 15 2003.
 *  Copyright (c) 2003 LAPLJ. All rights reserved.
 *
 */

#include "TagList.h"

using namespace std;
using namespace colibry;

// Prototypes (HELPERS)

string::size_type FindFirstNotInsideQuotes(const string& sea,
					   const string& body);

inline void db(const string& name, const string& val)
{ cout << name << ": [" << val << "]" << endl; }

//
// EXCEPTION HANDLING
//

XMLException::XMLException(int intype, const string& intag,
	     const string& inwhere) : Exception(intype,inwhere), m_tag(intag)
{
    switch (mType) {
    case TAGNOTFOUND:
	mWhat = "Tag not found";
	break;
    case UNCLOSEDTAG:
	mWhat = "Unclosed tag";
	break;
    case WRONGFORMAT:
	mWhat = "Wrong format";
	break;
    default:
	mWhat = "Unknown exception.";
    }
}


ostream& colibry::operator<<(ostream& os, const colibry::XMLException& e)
{
    return (os << "XMLException: " << e.what()
	    << "[" << e.m_tag << "]" << endl);
}

//
// TAGLIST
//

TagList::TagList()
{
}

TagList::~TagList()
{
    Clear();
}

void TagList::Clear()
{
    for (iterator i=begin(); i!=end(); i++)
	delete *i;
    clear(); // empty list
}

// GETTAG
// Gets tagno-th occurence of tpath

Tag* TagList::GetTag(const TagPath& tpath, int tagno)
{
    if (tpath.empty())
	return NULL;

    TagPath path(tpath.begin()+1,tpath.end());	// skip first
    Tag* tag;
    TagList* subtags;
    for (iterator i=begin(); i!=end(); i++) {
	tag = *i;
	if (tag->GetName() == tpath[0]) {
	    if (path.empty()) {
		// found
		if (tagno == 0)
		    return tag;
		else
		    tagno--;
	    } else {
		subtags = tag->GetSubtagList();
		if ((tag = subtags->GetTag(path,tagno)) != NULL) {
		    // found
		    return tag;
		}
	    }
	}
    }
    return NULL;
}

Tag* TagList::GetTag(const std::string& tpath, int tagno)
{
    // Parse tpath ("TAG/SUBTAG1/SUBSUBTAG")
    TagPath tp;
    string::size_type pos1,pos2;
    pos1 = 0;
    pos2 = tpath.find("/");
    while (pos2 != string::npos) {
	tp.push_back(tpath.substr(pos1,pos2-pos1));
	pos1 = pos2+1;
	pos2 = tpath.find("/",pos1+1);
    }
    tp.push_back(tpath.substr(pos1));

    return GetTag(tp,tagno);
}


ostream& colibry::operator<<(ostream& os, colibry::TagList& xml)
{
    TagList::iterator i;
    for (i = xml.begin(); i!=xml.end(); i++)
	os << *(*i) << endl;
    return os;
}


istream& operator>>(istream& is, TagList& xml)
{
    // Read entire input stream into "input"
    string input;
    char c;
    c = is.get();
    while (!is.eof()) {
	input += c;
	c = is.get();
    }
    // is.clear();

    // Parse "input"
    try {
	xml.ParseXMLString(input);
    } catch (colibry::XMLException& e) {
	xml.Clear();
	// is.setstate(ios::failbit);
	throw;
    }

    return is;
}


void TagList::ParseXMLString(string& xmlstr)
{
    // Doesn't require that the self-closed tags have " " before "/>"...
    // Comments are discarded (and therefore not saved or printed out...)

    Tag* tag;
    string::size_type pos1, pos2, endtag;
    bool is_self_closed = false;

    Clear();    // clears current TagList

    // ----- Find opening tag

    pos1 = xmlstr.find("<");
    while (pos1 != string::npos) {

	// check if opening comments...
	if (xmlstr.substr(pos1,4) == "<!--") {
	    endtag = xmlstr.find("-->",pos1);
	    if (endtag == string::npos)
		throw XMLException(XMLException::WRONGFORMAT,
				   "tag","TagList::ParseXMLString");
	    xmlstr.erase(pos1,endtag-pos1+3);
	    pos1 = xmlstr.find("<");
	    continue;
	}

	pos2 = FindFirstNotInsideQuotes(">",xmlstr);
	if (pos1 == string::npos || pos2 == string::npos)
	    throw XMLException(XMLException::WRONGFORMAT,"tag","TagList::ParseXMLString");
	if (xmlstr[pos2-1] == '?' || xmlstr[pos2-1] == '/')
	    pos2--;
	string open_tag(xmlstr.substr(pos1+1,pos2-pos1-1));

	// ----- Analyse opening tag

	// Parse name + parameters
	string tag_name, par_name, par_value;

	// Find name
	pos1 = open_tag.find(" ");
	tag_name = open_tag.substr(0,pos1);

	tag = new Tag(tag_name);

	// ----- Find body tag

	string body;
	if (xmlstr[pos2] != '/' && xmlstr[pos2] != '?') {
	    // there is some body
	    is_self_closed = false;
	    endtag = xmlstr.find(tag->GetClosingStr(),pos2);
	    if (endtag == string::npos) {
		throw XMLException(XMLException::UNCLOSEDTAG,tag_name,
				   "TagList::ParseXMLString");
	    }
	    body = xmlstr.substr(pos2+1,endtag-pos2-1);
	    endtag += (tag->GetClosingStr()).size() + 1;
	} else {
	    is_self_closed = true;
	    body.clear();
	    endtag = pos2+3;
	}

	// Parse parameters

	if (pos1 != string::npos) {
	    // pos1 points to " " => has parameters
	    pos2 = open_tag.find("=",pos1);
	    while (pos2 != string::npos) {
		par_name = open_tag.substr(pos1+1,pos2-pos1-1);
		pos1 = pos2+1;
		if (open_tag[pos1] == '\"') {
		    // quotes
		    pos2 = open_tag.find('\"',pos1+1);   // find closing quote
		    if (pos2 == string::npos)
			throw XMLException(XMLException::WRONGFORMAT,tag_name,
					   "TagList::ParseXMLString");
		    par_value = open_tag.substr(pos1+1,pos2-pos1-1);
		    pos1 = pos2+1;
		    pos2 = open_tag.find("=",pos1);
		} else {
		    // no quotes
		    pos2 = open_tag.find(" ",pos1);  // find space
		    if (pos2 == string::npos)
			par_value = open_tag.substr(pos1,pos2);
		    else {
			par_value = open_tag.substr(pos1,pos2-pos1);
			pos1 = pos2;
			pos2 = open_tag.find("=",pos1);
		    }
		}

		tag->AddParameter(par_name, par_value);
	    }
	}

	if (!is_self_closed) {
	    // Parse body
	    pos1 = body.find("<");
	    if (pos1 == string::npos) {
		// simple data body, with no nested tags
		tag->SetData(body);
	    } else {
		// nested (recursive)
		(tag->GetSubtagList())->ParseXMLString(body);
	    }
	}

	push_back(tag);

	xmlstr.erase(0,endtag-1);

	pos1 = xmlstr.find("<");
    }
}

//
// HELPERS
//

string::size_type FindFirstNotInsideQuotes(const string& sea,
					   const string& body)
{
    int count=0;
    string::size_type pos1 = 0;
    string::size_type candidate = 0;

    candidate = body.find_first_of(sea);
    while (candidate != string::npos) {

	pos1 = body.find("\"",pos1);
	while (pos1 < candidate && pos1 != string::npos) {
	    count++;
	    pos1 = body.find("\"",pos1+1);
	}

	if (count % 2 == 0)
	    return candidate;
	candidate = body.find_first_of(sea,candidate+1);
    }
    return candidate;
}
