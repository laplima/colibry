#include "PSemaphore.h"
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <semaphore.h>
#include <format>
// #include <sstream>
#include <random>
#include "../Throw_if/throw_if.h"
#include <cerrno>

void colibry::delete_sem(const std::string& id)
{
	sem_unlink(ipc::fixn(id).c_str());
}

// this constructor will not create a new semaphore
// (and, therefore, should not unlink it upon destruction)
colibry::Semaphore::Semaphore(const std::string& id)
	: _name{ipc::fixn(id)},
	_sem{::sem_open(_name.c_str(),O_RDWR)},
	_created{false}, _persist{true}
{
	throw_if(_sem==SEM_FAILED,"sem_open(" << id << ")");
}

colibry::Semaphore::Semaphore(Semaphore&& s) noexcept
	: _name{std::move(s._name)},
	_sem{s._sem},
	_created{s._created},
	_persist{s._persist}
{
	s._name.clear();
	s._sem = nullptr;
	s._created = false;
}


colibry::Semaphore::Semaphore(const std::string& id, int val, bool persist, mode_t mode)
	: _name{ipc::fixn(id)},
	_sem{::sem_open(_name.c_str(),O_RDWR|O_CREAT|O_EXCL,mode,val)},
	_created{true}, _persist{persist}
{
	if (_sem == SEM_FAILED && errno == EEXIST) {
		_sem = ::sem_open(_name.c_str(),O_RDWR);
		_created = false;
	}
	require(_sem!=SEM_FAILED, std::format("sem_open({})-create",id));
}

colibry::Semaphore::~Semaphore()
{
	if (_created && !_persist)
		delete_sem(_name);
}

void colibry::Semaphore::up()
{
	colibry::require(_sem != nullptr, std::format("{}.up() failed", _name));
	::sem_post(_sem);
}

void colibry::Semaphore::down()
{
	colibry::require(_sem != nullptr, std::format("{}.down() failed", _name));
	::sem_wait(_sem);
}

void colibry::Semaphore::down(float t)
{
	// timed wait
	colibry::require(_sem != nullptr, std::format("{}.down() failed", _name));
	struct timespec ts;
	ts.tv_sec = static_cast<int>(t);
	ts.tv_nsec = static_cast<int>((t-ts.tv_sec) * 1e9);
	::sem_timedwait(_sem, &ts);
}

bool colibry::Semaphore::try_down()
{
	if (_sem == nullptr)
		return false;
	return (::sem_trywait(_sem) == 0); // errno = EAGAIN => sem == 0
}

// helpers

std::string colibry::ipc::make_uids(unsigned short len)
{
	std::random_device r;
	std::default_random_engine gen{r()};
	std::uniform_int_distribution<int> uid{0,255};

	std::ostringstream id;
	const int sz = static_cast<int>(len/2.0 + 0.5);
	for (int i=0; i<sz; ++i)
		id << std::setw(2) << std::setfill('0') << std::hex << uid(gen);
	return id.str().substr(0,len);
}

// prefix "/", if necessary
std::string colibry::ipc::fixn(const std::string_view s)
{
	if (s[0] == '/')
		return s.data();
	return std::string{"/"} + s;
}
