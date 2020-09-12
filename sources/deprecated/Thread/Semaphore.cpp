#include <Semaphore.h>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace colibry;

Semaphore::Semaphore(int val)
{
    SetVal(val);
}

Semaphore::~Semaphore()
{
    Destroy();
}

void Semaphore::SetVal (int value)
{
    v = value;
    if (pthread_mutex_init (&mutex, NULL/*pthread_mutexattr_default*/) == -1)
	do_error ("Error setting up semaphore mutex");

    if (pthread_cond_init (&cond, NULL /*pthread_condattr_default*/) == -1)
	do_error ("Error setting up semaphore condition signal");
}

/*
 * function should be called when there is no longer a need for
 * the semaphore.
 *
 */
void Semaphore::Destroy ()
{
    if (pthread_mutex_destroy (&mutex) == -1)
     do_error ("Error destroying semaphore mutex");

    if (pthread_cond_destroy (&cond) == -1)
     do_error ("Error destroying semaphore condition signal");
}

/*
 * function increments the semaphore and signals any threads that
 * are blocked waiting a change in the semaphore.
 *
 */
int Semaphore::Up ()
{
    int value_after_op;

    tw_pthread_mutex_lock (&mutex);

    v++;
    value_after_op = v;

    tw_pthread_mutex_unlock (&mutex);
    tw_pthread_cond_signal (&cond);

    return (value_after_op);
}

/*
 * function decrements the semaphore and blocks if the semaphore is
 * <= 0 until another thread signals a change.
 *
 */
int Semaphore::Down ()
{
    int value_after_op;

    tw_pthread_mutex_lock (&mutex);
    while (v <= 0)
	tw_pthread_cond_wait (&cond, &mutex);
    
    v--;
    value_after_op = v;
    tw_pthread_mutex_unlock (&mutex);
    
    return (value_after_op);
}

/*
 * function does NOT block but simply decrements the semaphore.
 * should not be used instead of down -- only for programs where
 * multiple threads must up on a semaphore before another thread
 * can go down, i.e., allows programmer to set the semaphore to
 * a negative value prior to using it for synchronization.
 *
 */
int Semaphore::Decrement ()
{
    int value_after_op;

    tw_pthread_mutex_lock (&mutex);
    v--;
    value_after_op = v;
    tw_pthread_mutex_unlock (&mutex);

    return (value_after_op);
}

/*
 * function returns the value of the semaphore at the time the
 * critical section is accessed.  obviously the value is not guarenteed
 * after the function unlocks the critical section.  provided only
 * for casual debugging, a better approach is for the programmar to
 * protect one semaphore with another and then check its value.
 * an alternative is to simply record the value returned by semaphore_up
 * or semaphore_down.
 *
 */
int Semaphore::GetVal ()
{
    /* not for sync */
    int value_after_op;

    tw_pthread_mutex_lock (&mutex);
    value_after_op = v;
    tw_pthread_mutex_unlock (&mutex);

    return (value_after_op);
}



/* -------------------------------------------------------------------- */
/* The following functions replace standard library functions in that   */
/* they exit on any error returned from the system calls.  Saves us     */
/* from having to check each and every call above.                      */
/* -------------------------------------------------------------------- */


int Semaphore::tw_pthread_mutex_unlock (pthread_mutex_t * m)
{
    int         return_value;

    if ((return_value = pthread_mutex_unlock (m)) == -1)
	do_error ("pthread_mutex_unlock");

    return (return_value);
}

int Semaphore::tw_pthread_mutex_lock (pthread_mutex_t * m)
{
    int return_value;

    if ((return_value = pthread_mutex_lock (m)) == -1)
	do_error ("pthread_mutex_lock");
    
    return (return_value);
}

int Semaphore::tw_pthread_cond_wait (pthread_cond_t * c, pthread_mutex_t * m)
{
    int return_value;

    if ((return_value = pthread_cond_wait (c, m)) == -1)
	do_error ("pthread_cond_wait");
    
    return (return_value);
}

int Semaphore::tw_pthread_cond_signal (pthread_cond_t * c)
{
    int return_value;

    if ((return_value = pthread_cond_signal (c)) == -1)
	do_error ("pthread_cond_signal");
    
    return (return_value);
}


/*
 * function just prints an error message and exits 
 *
 */
void Semaphore::do_error (const char *msg)
{
    cerr << "ERROR:" << msg << endl;
    exit (1);
}
