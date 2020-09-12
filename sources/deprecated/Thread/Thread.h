//
// Thread Class - pthread wrapper
//
// (C) 2006 by LAPLJ. All rights reserved.
//
// Classe T deve possuir copy constructor + assignment operator
//

#ifndef __THREADS_H__
#define __THREADS_H__

#include <pthread.h>
#include <string>

namespace colibry {

	class Thread {
	public:
		Thread();
		Thread(const char* name);
		virtual ~Thread();

		void Start();
		void Wait();
		void Detach();
		void Exit();

		virtual int GetState();
		pthread_t Id() { return m_tid; }
		const char* Name() { return m_name.c_str(); }

	protected:
		virtual void Run() = 0;

	private:
		static void* Exec(void*);

	private:
		pthread_t m_tid;
		std::string m_name;

		// Prevent copy construction + assignment
		Thread(const Thread&) {}
		Thread& operator=(const Thread&) { return *this; }
	};

};  // end namespace

#endif
