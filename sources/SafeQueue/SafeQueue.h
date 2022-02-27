//
// Thread-safe Queue
//
// (C) 2011-22 LAPLJ. All rights reserved.
//

#include <queue>
#include <semaphore>
#include <mutex>
#include <stdexcept>

namespace colibry {

    template <typename T, int max>
	class SafeQueue {
	public:
		virtual void insert(const T& x);
		virtual T remove();
		virtual bool try_remove(T& x);
		virtual T try_remove();
		virtual void clean();
	private:
		std::queue<T> data_;
		mutable std::mutex mutex_{};
		mutable std::counting_semaphore<max> full_{0};
	};

	// implementation

	template<typename T, int max>
	void SafeQueue<T,max>::insert(const T& x)
	{
		{
			std::lock_guard<std::mutex> g{mutex_};
			data_.push(x);
		}
		full_.release();
	}

	template<typename T, int max>
	T SafeQueue<T,max>::remove()
	{
		full_.acquire();
		std::lock_guard<std::mutex> g{mutex_};
		T aux = data_.front();
		data_.pop();
		return aux;
	}

	template<typename T, int max>
	bool SafeQueue<T,max>::try_remove(T& x)
	{
		std::lock_guard<std::mutex> g{mutex_};
		if (data_.empty())
			return false;
	    full_.acquire(); // won't bacquire
	    x = data_.front();
	    data_.pop();
	    return true;
	}

	template<typename T, int max>
	T SafeQueue<T,max>::try_remove()
	{
		std::lock_guard<std::mutex> g{mutex_};
		if (data_.empty())
			throw std::underflow_error{"empty SafeQueue"};
		full_.acquire();
		T aux = data_.front();
		data_.pop();
		return aux;
	}

	template<typename T, int max>
	void SafeQueue<T,max>::clean()
	{
		std::lock_guard<std::mutex> g{mutex_};
		std::queue<T> e;
		data_.swap(e);
	}

} // namespace colibry
