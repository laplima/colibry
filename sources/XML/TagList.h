/*
 *  TagList.h
 *  
 *
 *  Created by Luiz Lima Jr. on Tue Jul 15 2003.
 *  Copyright (c) 2003 LAPLJ. All rights reserved.
 *
 */

#ifndef __TAGLIST_H__
#define __TAGLIST_H__

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <Exception.h>  // colibry
#include "Tag.h"

namespace colibry {

    // XML exception

    class XMLException : public Exception {
    public:
	
	enum Type { TAGNOTFOUND, UNCLOSEDTAG, WRONGFORMAT, MEMORY };

    public:

	XMLException(int type, const std::string& intag,
		     const std::string& inwhere="");
	virtual ~XMLException() throw() {}

	std::string tag() { return m_tag; }
	friend std::ostream& operator<<(std::ostream& os, const XMLException& e);
	
    private:

	std::string m_tag;
    };
    

    // Types
    
    typedef std::vector<std::string> TagPath;

    // TAGLIST
    
    class TagList : public std::list<Tag*>
    {
    public:
            
	TagList();
	virtual ~TagList();
	
	virtual void Clear();
	Tag* GetTag(const TagPath& tpath, int tagno=0);
	Tag* GetTag(const std::string& tpath, int tagno=0);

	void ParseXMLString(std::string& xmlstring) throw (XMLException);
	
	friend std::ostream& operator<<(std::ostream& os, TagList& xml);
	friend std::istream& operator>>(std::istream& is, TagList& xml)
	    throw (XMLException);

    protected:
	
	int m_tagno;
	TagList(const TagList&) {}
	TagList& operator=(const TagList&) { return *this; }
    };
};

#endif

