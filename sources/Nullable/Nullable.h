//
// Nullable
// (C) 2011 by LAPLJ. All rights reserved.
//
//  Required methods for class ANY:
//   - copy constructor
//   - assignment operator
//   - operator==
//

#ifndef __NULLABLE_H__
#define __NULLABLE_H__

namespace colibry {
    
    class IsNullExc {};

    template <class ANY>
    class Nullable {
    public:
	
	Nullable() : m_value(NULL) {}
	Nullable(const Nullable& na) { *this = na; }
	Nullable(const ANY& v) { m_value = new ANY(v); }
	virtual ~Nullable() { clean(); }
    
	Nullable& operator=(const Nullable& na)
	{
	    if (this != &na) {
		clean();
		m_value = (na.m_value==NULL) ? NULL : new ANY(*na.m_value);  // deep copy
	    }
	    return *this;
	}
    
	Nullable& operator=(const ANY& v)
	{
	    clean();
	    m_value = new ANY(v);
	    return *this;
	}
    
	bool operator==(const Nullable<ANY>& n) const
	{
	    if (this->IsNull())
		return n.IsNull();
	    else if (n.IsNull())
		return false;
	    else
		return (bool)(*m_value == *(n.m_value));
	}

	bool operator==(const ANY& v) const
	{
	    if (this->IsNull())
		return false;
	    else
		return (*m_value == v);
	}
    
	bool operator!=(const Nullable<ANY>& n) const { return !(*this == n); }
	bool operator!=(const ANY& v) const { return !(*this == v); }
    
	ANY& operator*() throw (IsNullExc)
	{
	    if (IsNull())
		throw IsNullExc();
	    return *m_value;
	}
    
	ANY* operator->() throw (IsNullExc) { return &(this->operator*()); }

	ANY* ptr() { return m_value; }

	ANY get() const
	{
	    if (IsNull()) throw IsNullExc();
	    return *m_value;
	}
	
	ANY get_copy() const { return get(); }
    
	bool IsNull() const { return (m_value==NULL); }
	//operator bool() const { return !IsNull(); }
	operator ANY() { return get(); }

	void clean() { MakeNull(); }

	void MakeNull()
	{
	    if (m_value != NULL) {
		delete m_value;
		m_value = NULL;
	    }
	}
	
	static Nullable Null() { return Nullable<ANY>(); }

    private:
    
	ANY* m_value;
    };
    
}  // namespace

#endif
