#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <pthread.h>

namespace colibry {

    class Semaphore {
    public:

	Semaphore() { SetVal(0); }
	Semaphore(int value);
	virtual ~Semaphore();

	/* principais */
	void SetVal (int value); /* atribui valor a semaforo */
	int Down ();
	int Up ();
    
	/* auxiliares */
	int Decrement ();
	int GetVal ();
	int tw_pthread_cond_signal (pthread_cond_t * c);
	int tw_pthread_cond_wait (pthread_cond_t * c, pthread_mutex_t * m);
	int tw_pthread_mutex_unlock (pthread_mutex_t * m);
	int tw_pthread_mutex_lock (pthread_mutex_t * m);

    private:
	
	void Destroy ();         /* destroi semaforo */
	void do_error (const char *msg);

    private:

	int v;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
    };    
};
    
#endif
