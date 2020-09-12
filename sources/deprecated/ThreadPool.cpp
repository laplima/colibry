#include <iostream>
#include <vector>
#include "ThreadPool.h"

using namespace std;
using namespace colibry;

Worker::Worker(CORBA::ORB_ptr orb) throw (ThreadCreationExc)
{
    if (pthread_create(&m_tid,NULL,Worker::ORBThread,(void*)orb) != 0)
	throw ThreadCreationExc();
}

void* Worker::ORBThread(void* args)
{
    CORBA::ORB_var orb = CORBA::ORB::_duplicate(static_cast<CORBA::ORB_ptr>(args));
    try {
	orb->run();
    } catch(...) {}
    return NULL;
}

// Thread pool

ThreadPool::ThreadPool(CORBA::ORB_ptr orb, unsigned short count)
{
    try {
	for (int i=0; i<count; i++)
	    m_workers.push_back(new Worker(orb));
    } catch (ThreadCreationExc& ) {
	// thread limit reached
	cerr << "Limit = " << m_workers.size() << " reached ";
    }
}

ThreadPool::~ThreadPool()
{
    join();
}


void ThreadPool::join()
{
    // Wait for all threads in gWorkers to finish
    while (!m_workers.empty()) {
	pthread_join(m_workers[0]->Id(),NULL);
	m_workers.erase(m_workers.begin());
    }
}
