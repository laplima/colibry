#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <colibry/throw_if.h>
#include <colibry/PSemaphore.h>		// for fix() and make_uid()

namespace colibry {

	template <typename T>
	T* create_shm(const std::string& id, size_t n, bool& created, mode_t mode=0666)
	{
		auto oldmode = umask(~mode & 0777);
		int fd = shm_open(ipc::fixn(id).c_str(),O_RDWR|O_CREAT|O_EXCL,mode);
		umask(oldmode);
		if (fd == -1) {
			if (errno == EEXIST) {
				fd = shm_open(ipc::fixn(id).c_str(),O_RDWR,mode);
				created = false;
			} 
            if (fd == -1) perror("shm_open");
			throw_if(fd==-1,"shm_open(" << id << ") " << errno);
		} else {
			created = true;
			int err = ftruncate(fd,n*sizeof(T));
			throw_if(err==-1,"ftruncate(" << sizeof(T) << ")");
		}
		T* shmptr = static_cast<T*>(mmap(nullptr,n*sizeof(T),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0));
		throw_if(shmptr==MAP_FAILED,"mmap()");
		return shmptr;
	}

	template <typename T>
	T* create_shm(const std::string& id, size_t n=1, mode_t mode=0666)
	{
		bool created = false;
		return create_shm<T>(id,n,created,mode);
	}

	template <typename T>
	T* create_shm(const std::string& id, bool& created, mode_t mode=0666)
	{
		return create_shm<T>(id,1,created,mode);
	}

	void delete_shm(const std::string& id)
	{
		throw_if(shm_unlink(ipc::fixn(id).c_str())==-1,"delete_shm(" << id << ")");
	}

	//
	// SharedMemory
	//
	// When name is not provided, generate a random name.
	//
	// Usage:
	//		SharedMemory<char,64> shm{"str"};		// 64*sizeof(char)
	//		strcpy(static_cast<char*>(shm),"hello");
	//

	template <typename T, std::size_t N=1>
	class SharedMemory {
	public:
		SharedMemory(bool persist = false)
			: persist_{persist}, m_size{N}, m_name{make_name()} {
				init();
		}
        SharedMemory(const char* name, bool persist = false)
        	: persist_{persist}, m_size{N}, m_name{name} {
        	init();
        }  // needed since char* maps to bool...
		SharedMemory(std::string name, bool persist = false) : persist_{persist}, m_size{N}, m_name{std::move(name)} { init(); }
		SharedMemory(const SharedMemory<T,N>& shm) = delete;
		SharedMemory(SharedMemory<T,N>&& shm) = delete;
		virtual ~SharedMemory() {
			if (not persist_)
				delete_shm(m_name);
		}

		//SharedMemory& operator=(const SharedMemory& shm) = delete;	// to do later

		[[nodiscard]] virtual size_t size() const { return m_size; }
		[[nodiscard]] virtual size_t bytes() const { return m_size * sizeof(T); }

		[[nodiscard]] virtual std::string name() const { return m_name; }
		[[nodiscard]] virtual bool created() const { return created_; }
		[[nodiscard]] virtual bool persist() const { return persist_; }

		virtual T* data() { return m_pointer; }
		virtual T& get() { return *m_pointer; }
		virtual operator T&() { return *m_pointer; }
		virtual operator T&() const { return *m_pointer; }
		virtual explicit operator T*() { return m_pointer; }

		SharedMemory& operator=(const T& x) {
			if (m_pointer!= nullptr) *m_pointer = x;
			return *this;
		}

	protected:
		void init() {
			m_pointer = nullptr;
			m_pointer = create_shm<T>(m_name,m_size,created_);
		}
		static std::string make_name(){ return ipc::make_uids(16); }

		bool persist_;
		bool created_ = false;
		T* m_pointer;
		size_t m_size;
		std::string m_name;
	};

};

#endif
