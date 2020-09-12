/*
 *  Tag.h
 *
 *  Created by Luiz Lima Jr. on Fri Mar 08 2002.
 *  Copyright (c) 2003 LAPLJ. All rights reserved.
 *
 */

#ifndef __TAG_H__
#define __TAG_H__

#include <iostream>
#include <list>
#include <map>
#include <string>

namespace colibry
{
    
    // Types
    
    typedef std::map<std::string,std::string> ParameterMap;
    class TagList;	// advanced declaration
    enum TagType { SIMPLE, NESTED, SELF_CLOSED };
    
    // TAG
    
    class Tag {
    protected:
	
	std::string  m_name;		// TAG name
	std::string* m_data;		// TAG data (NULL if <TAG ... />)
	ParameterMap m_par_map;		// Parameter map
	TagList*     m_subtags;		// Sub-tag list
	TagType	     m_type;
	
	Tag(const Tag& tag);		        // Copy constructor (to avoid)
	virtual Tag& operator=(const Tag& tg)	// assignment (to avoid)
	{ return *this; }
      
    public:
	
	Tag(const std::string& tagname);
	Tag(const std::string& tagname, const std::string& data); // Data tag
	virtual ~Tag();
	
	virtual void AddSubtag(Tag* child);
	virtual void AddParameter(const std::string& par,
				  const std::string& value);
	virtual void SetData(const std::string& data);
	virtual void SetTagName(const std::string& tagname);
	
	virtual bool HasData() { return (bool) (m_data != NULL); }
	virtual TagType Type();
	virtual std::string GetName() { return m_name; }
	virtual std::string GetData();
	virtual TagList* GetSubtagList() { return m_subtags; }
	virtual ParameterMap* GetParamMap() { return &m_par_map; }
	virtual std::string GetParameterValue(const std::string& par);
	
	virtual std::string GetOpeningStr();
	virtual std::string GetClosingStr();
	
	friend std::ostream &operator<<(std::ostream& os, Tag& tag);
	
	// STATIC FUNCTIONS

	static std::string MakeOpeningTag(const std::string& tagname,
					  ParameterMap* pmap=NULL);
	static std::string MakeClosingTag(const std::string& tagname);
	static std::string MakeSelfClosedTag(const std::string& tagname,
					     ParameterMap* pmap=NULL);
    };
};

#endif
