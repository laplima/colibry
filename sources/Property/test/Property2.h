#ifndef __PROPERTY2_H__
#define __PROPERTY2_H__

template<typename T, char PropertyType>
class Property {
private:
	T myVar;    
public:    
	Property() {
		if (PropertyType != 'r' && PropertyType != 'w' && PropertyType != 'b')
			throw "Invalid PropertyType. PropertyTypes allowed are :: \n\tr ->Read , w -> Write , b -> Both Read and Write.";
	}
	operator T() {
		return GetValue();
	}
	void operator= (T theVar) {
		SetValue(theVar);
	}
	virtual T GetValue() {
		if (PropertyType == 'w')
			throw "Cannot read from a write-only property";
		return myVar;
	}
	virtual void SetValue(T theVar) {
		if ( PropertyType == 'r' )
			throw "Cannot write to a read-only property";        
		myVar = theVar;
	}
};

#endif
