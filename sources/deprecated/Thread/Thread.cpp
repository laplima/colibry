//
// Thread Class - pthread wrapper
//
// (C) 2006 by LAPLJ. All rights reserved.
//
// Classe T deve possuir copy constructor + assignment operator
//

#include "Thread.h"

using namespace colibry;

Thread::Thread()
{
	m_name = "Thread";
}

Thread::Thread(const char* name)
{
	if (name == NULL)
		m_name = "Thread";
	else
		m_name = name;
}

Thread::~Thread()
{
}

void Thread::Start()
{
	pthread_create(&m_tid, NULL, Thread::Exec, (void*)this);
}

void Thread::Wait()
{
	pthread_join(m_tid,NULL);
}

void Thread::Detach()
{
	pthread_detach(m_tid);
}

void Thread::Exit()
{
	pthread_exit(NULL);
}

int Thread::GetState()
{
	return 0;
}

void* Thread::Exec(void* args)
{
	Thread* thread = static_cast<Thread*>(args);
	thread->Run();
	return NULL;
}
