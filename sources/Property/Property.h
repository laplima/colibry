//
// From CodeGuru article
//
// http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4031/
//
// Adapted by LAPLJ.
// Jan. 2008-2011
//
// Declaration:
//
// 		Property<Test,int,'a'> prop;
//
// Possible types:
// 		- 'r' = read-only
//		- 'w' = write-only
//		- 'a' = read+write
//
// Setup:
//
//		prop.setContainer(this);
// 		prop.setter(setValue);
// 		prop.getter(getValue);
//
// Setter must have one and only one const parameter of type ValueType, e.g.
// (ValueType = int)
//
//	  void setValue(const int v) { ... }
//
// Getter must return ValueType, e.g.
//
//	  int getValue() const { ... }
//

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <iostream>
#include <string>
#include <stdexcept>

namespace colibry {

	//
	// PropException
	//

	class PropException : std::runtime_error {
	public:
		PropException(const std::string& w) : std::runtime_error{w} {}
	};

	//
	// Property
	//

	template<typename Container, typename ValueType, char PropType>
	class Property {
	public:
		Property() : _cobj(nullptr), _get(nullptr), _set(nullptr)
		{
			if (PropType != 'r' && PropType != 'w' && PropType != 'a')
				throw PropException("Invalid property type (only 'r', 'w' or 'a' are allowed)");
		}

		Property(Container* cObject, ValueType(Container::*pGet)() const,
			void (Container::*pSet)(const ValueType))
		: _cobj(cObject), _get(nullptr), _set(nullptr)
		{
			if (PropType != 'r' && PropType != 'w' && PropType != 'a')
				throw PropException("Invalid property type (only 'r', 'w' or 'a' are allowed)");
			setter(pSet);
			getter(pGet);
		}

		void setContainer(Container* cObject)
		{
			_cobj = cObject;
		}

		void setter(void (Container::*pSet)(const ValueType value))
		{
			if ((PropType == 'w') || (PropType == 'a'))
				_set = pSet;
		}

		void getter(ValueType(Container::*pGet)() const)
		{
			if ((PropType == 'r') || (PropType == 'a'))
				_get = pGet;
		}

		ValueType operator=(const ValueType value)
		{
			if (_cobj == nullptr) throw PropException("No property container");
			if (_set == nullptr) throw PropException("No property setter");
			(_cobj->*_set)(value);
			return value;
		}

	operator ValueType() const // cast to internal type
	{
		if (_cobj == nullptr) throw PropException("No property container");
		if (_get == nullptr) throw PropException("No property getter");
		return (_cobj->*_get)();
	}
private:
	Container* _cobj;  // pointer to the module that contains the property
	void (Container::*_set)(const ValueType value); // ptr to set member function
	ValueType (Container::*_get)() const; // ptr to get member function
};

};

#endif
