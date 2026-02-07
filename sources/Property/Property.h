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
// 		Property<Test,int,PropType::rdwr> prop;
//
// Possible types:
// 		- PropType::rd = read-only
//		- PropType::wr = write-only
//		- PropType::rdwr = read+write
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

#ifndef PROPERTY_H
#define PROPERTY_H

#include <iostream>
#include <string>
#include <stdexcept>

namespace colibry {

	//
	// PropException
	//

	class PropException : public std::runtime_error {
	public:
		PropException(const std::string& w) : std::runtime_error{w} {}
	};

	//
	// Property
	//

	enum class PropType : char { rd, wr, rdwr };

	template<typename Container, typename ValueType, PropType PropType=PropType::rdwr>
	class Property {
	public:
		Property() : _cobj(nullptr), _get(nullptr), _set(nullptr)
		{}

		Property(Container* cObject,
			ValueType(Container::*pGet)() const,
			void (Container::*pSet)(const ValueType))
		: _cobj(cObject), _get(nullptr), _set(nullptr)
		{
			setter(pSet);
			getter(pGet);
		}

		void setContainer(Container* cObject)
		{
			_cobj = cObject;
		}

		void setter(void (Container::*pSet)(const ValueType value))
		{
			if ((PropType == PropType::wr) || (PropType == PropType::rdwr))
				_set = pSet;
		}

		void getter(ValueType(Container::*pGet)() const)
		{
			if ((PropType == PropType::rd) || (PropType == PropType::rdwr))
				_get = pGet;
		}

		// ValueType operator=(const ValueType value)
		// {
		// 	if (_cobj == nullptr) throw PropException{"No property container"};
		// 	if (_set == nullptr) throw PropException{"No property setter"};
		// 	(_cobj->*_set)(value);
		// 	return value;
		// }

		Property& operator=(const ValueType value)
		{
			if (_cobj == nullptr) throw PropException{"No property container"};
			if (_set == nullptr) throw PropException{"No property setter"};
			(_cobj->*_set)(value);
			return *this;
		}

		operator ValueType() const // cast to internal type
		{
			if (_cobj == nullptr) throw PropException("No property container");
			if (_get == nullptr) throw PropException("No property getter");
			return (_cobj->*_get)();
		}
	private:
		Container* _cobj;  // pointer to the module that contains the property
		ValueType (Container::*_get)() const; // ptr to get member function
		void (Container::*_set)(const ValueType value); // ptr to set member function
	};

};

#endif
