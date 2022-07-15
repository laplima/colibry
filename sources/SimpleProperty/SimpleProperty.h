#ifndef SIMPLEPROPERTY_H
#define SIMPLEPROPERTY_H

//
// This is a simple implementation of a property
// (prefferable over complex Propery.h)
//
// USAGE (override setter + getter):
//
// template <typename T>
// class MyProp : public SimpleProperty<T> {
// public:
//     MyProp(const T& v) : SimpleProperty<T>{v} {}
// protected:
//      void setter(const T& v) {
// 	        std::cout << "setter: " << v << std::endl;
// 	        SimpleProperty<T>::setter(v);
// 	    }
//      const T& getter() {
// 	        std::cout << "getter" << std::endl;
// 	        return SimpleProperty<T>::getter();
//      }
// };
//
//       MyProp<int> mp{123};
//       cout << mp << endl;
//       mp = 256;
//       cout << mp << endl;
//

namespace colibry {

	template<typename T>
	class SimpleProperty {
	public:
		SimpleProperty() {}
		SimpleProperty(const T& initial) : value_{initial} {}
	public:
		SimpleProperty<T>& operator=(const T& t) { setter(t); return *this; } // set
		operator const T() { return value_; }							// get
	protected:
		// to verride
		virtual void setter(const T& v) { value_ = v; }
		virtual const T& getter() { return value_; }
	protected:
		T value_;
	};

}

#endif

