//
// DEPRECATED
//
// C++11 "counting" semaphore implementation using condition variables
// (C) 2013-2020 by laplima
//
// It seems that C++20 has "counting_sepmaphore" class...
//

#ifndef __SEMAPHORE_C11__
#define __SEMAPHORE_C11__

#include <mutex>
#include <condition_variable>

namespace colibry {

	class CSemaphore {
	public:
		CSemaphore(int value);
		void down();
		void up();
	private:
		int m_value;
		std::mutex m_mutex;
		std::condition_variable m_cond;
	};

}	// end namespace

#endif
