#include "PSemaphore.h"
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include "../Throw_if/throw_if.h"
#include <cerrno>

void colibry::delete_sem(const std::string& id)
{
	sem_unlink(ipc::fixn(id).c_str());
}

colibry::Semaphore::Semaphore(const std::string& id, bool persist)
	: _name{ipc::fixn(id)},
	_sem{::sem_open(_name.c_str(),O_RDWR)},
	_created{false}, _persist{persist}
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
	throw_if(_sem==SEM_FAILED,"sem_open(" << id << ")-create");
}

colibry::Semaphore::~Semaphore()
{
	if (_created && !_persist)
		delete_sem(_name);
}

void colibry::Semaphore::up()
{
	if (_sem != nullptr)
		sem_post(_sem);
}

void colibry::Semaphore::down()
{
	if (_sem != nullptr)
		sem_wait(_sem);
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
