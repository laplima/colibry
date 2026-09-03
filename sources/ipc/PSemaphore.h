//
// Semaphore is a wrapper for a POSIX semaphore
//
// While STL counting semaphores synchronize threads, PSemaphores can be used
// for inter-process synchronization.
//
// If persist == true, POSIX semaphore won't be unlinked upon object destruction
//

#ifndef PSEMAPHORE_H
#define PSEMAPHORE_H

#include <string>
#include <string_view>
#include <semaphore.h>

namespace colibry {

	void delete_sem(const std::string& id);

	class Semaphore {
	public:
		Semaphore(const std::string& id, bool persist=false);
		Semaphore(const std::string& id, int val, bool persist=false, mode_t mode=0666);
		Semaphore(const Semaphore&) = delete;
		Semaphore(Semaphore&&) noexcept;
		Semaphore& operator=(const Semaphore&) = delete;
		Semaphore& operator=(Semaphore&&) = delete;
		virtual ~Semaphore();
		virtual void up();
		virtual void down();
		virtual void persist(bool p=true) { _persist=p; }	// doesn't remove semaphore if true
		[[nodiscard]] virtual bool created() const { return _created; }
		[[nodiscard]] virtual bool persistent() const { return _persist; }
	private:
		std::string _name;
		sem_t* _sem;
		bool _created;
		bool _persist;
	};

	namespace ipc {
		std::string fixn(const std::string_view s);	// prepend '/'
		std::string make_uids(unsigned short len);	// generate random hex string
	}

};

#endif
