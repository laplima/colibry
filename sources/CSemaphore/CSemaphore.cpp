#include "CSemaphore.h"

using namespace colibry;

CSemaphore::CSemaphore(int value) : m_value{value}
{
}

void CSemaphore::down()
{
	std::unique_lock<std::mutex> lck{m_mutex};
	m_cond.wait(lck, [this] { return m_value>0; });
	--m_value;
}

void CSemaphore::up()
{
	std::unique_lock<std::mutex> lck{m_mutex};
	++m_value;
	m_cond.notify_one();
}
