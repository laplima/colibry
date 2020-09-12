#include <pthread.h>
#include <tao/corba.h>
#include <vector>

//
// Thread pool
//
// TAO does support thread pool semantics for the ORB_CTRL_MODEL POA policy.
// However, TAO requires that the application programmer create all the
// threads in the thread pool, and each of these threads must call orb->run(),
// which is how the application-level threads become part of the thread pool.

namespace colibry {

	//
	// WORKER - creates a thread and calls orb->run()
	//
	class ThreadCreationExc {};

	class Worker {
	public:
	    Worker(CORBA::ORB_ptr orb) throw (ThreadCreationExc);
	    pthread_t Id() const { return m_tid; }
	    static void* ORBThread(void* args);
	private:
	    pthread_t m_tid;
	};


	// Thread pool helpers
	class ThreadPool {
	public:

	    ThreadPool(CORBA::ORB_ptr orb, unsigned short count);
	    virtual ~ThreadPool();
	    void join();                     // wait for all threads in pool to terminate

	protected:

	    std::vector<Worker*> m_workers;
	};

};	// namespace
