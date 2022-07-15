#ifndef SINGLETON_H
#define SINGLETON_H

namespace colibry {

	template <class ANY>
	class Singleton {
	public:
		static ANY* getInstance();
		~Singleton() { m_singleton = nullptr; } // memory leak??
	protected:
		Singleton();
		Singleton(const Singleton&);
		Singleton& operator=(const Singleton&);
	private:
		static ANY *m_singleton;
	};

	template <class ANY>
	ANY* Singleton<ANY>::m_singleton = nullptr;

	template <class ANY>
	ANY* Singleton<ANY>::getInstance()
	{
		if (m_singleton == nullptr)
			m_singleton = new ANY();

		return m_singleton;
	}

}; // end namespace COLib

#endif
