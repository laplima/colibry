//
// Thread-safe Queue
//
// Depends on Semaphore library.
//
// (C) 2011 LAPLJ. All rights reserved.
//

#include <Semaphore.h>
#include <vector>

namespace colibry {

    class EmptyQueue {};

    template <class T>
    class SafeQueue {
    private:
	std::vector<T> _data;
	Semaphore _mutex;
	Semaphore _full;
    public:
	SafeQueue()
	{
	    _mutex.SetVal(1);
	    _full.SetVal(0);
	}
	virtual ~SafeQueue() {}
	virtual void insert(const T& x)
	{
	    _mutex.Down();
	    _data.push_back(x);
	    _mutex.Up();
	    _full.Up();
	}
	    
	virtual T remove()
	{
	    _full.Down();
	    _mutex.Down();
	    T aux = _data.front();
	    _data.erase(_data.begin());
	    _mutex.Up();
	    return aux;
	}
	
	virtual bool try_remove(T& x)
	{
	    _mutex.Down();
	    if (_data.size() == 0) {
		_mutex.Up();
		return false;
	    }
	    _full.Down(); // won't block
	    x = _data.front();
	    _data.erase(_data.begin());
	    _mutex.Up();
	    return true;
	}

	virtual T try_remove()
	{
	    _mutex.Down();
	    if (_data.size() == 0) {
		_mutex.Up();
		throw EmptyQueue();
	    }
	    _full.Down();
	    T aux = _data.front();
	    _data.erase(_data.begin());
	    _mutex.Up();
	    return aux;
	}
	
	virtual void clean()
	{
	    _mutex.Down();
	    _data.clear();
	    _full.SetVal(0);
	    _mutex.Up();
	}
	
    };
    
} // namespace
