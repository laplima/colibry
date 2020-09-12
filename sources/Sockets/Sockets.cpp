//
// (C) 2011-2020 LAPLJ.
// All rights reserved.
//

#include "Sockets.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sstream>
#include <throw_if.h>

using namespace std;
using namespace colibry;

constexpr int LISTENQUEUE = 1024;

inline void error(const char* msg)
{
    cerr << "SOCKET ERROR: " << msg << endl;
    exit(1);
}

// -------------------------------------------------------
// SocketBase
// -------------------------------------------------------

SocketBase::SocketBase()
    : FD(this,&SocketBase::getFD,nullptr),
      Type(this,&SocketBase::getType,nullptr),
      m_sockfd(-1), m_type(SocketType::UNDEFINED)
{
}

SocketBase::SocketBase(const SocketBase& sock)
    : FD(this,&SocketBase::getFD,nullptr),
      Type(this,&SocketBase::getType,nullptr)
{
    *this = sock;
}

SocketBase::~SocketBase()
{
}

SocketBase& SocketBase::operator=(const SocketBase& sock)
{
    if (this != &sock) {
		m_sockfd = sock.m_sockfd;
		m_type = sock.m_type;
    }
    return *this;
}

bool SocketBase::operator==(const SocketBase& sock)
{
    // ignore type...
    return (bool)(m_sockfd == sock.m_sockfd);
}

void SocketBase::Close()
{
    ::close(m_sockfd);
}

//
// -------------------------------------------------------
// Socket
// -------------------------------------------------------
//

Socket::Socket(const int fds)
    : RemoteHost(this,&Socket::getRemoteHost,nullptr),
      RemotePort(this,&Socket::getRemotePort,nullptr),
      m_is_connected(false), m_remote_port(0)
{
	m_type = SocketType::CLIENT_SOCK;
	m_sockfd = fds;
	throw_if(m_sockfd == -1, "Socket creation error");
}

Socket::Socket()
    : RemoteHost(this,&Socket::getRemoteHost,nullptr),
      RemotePort(this,&Socket::getRemotePort,nullptr),
      m_is_connected(false), m_remote_port(0)
{
    m_type = SocketType::CLIENT_SOCK;
    m_sockfd = ::socket(PF_INET,SOCK_STREAM,0);
	throw_if(m_sockfd == -1,"Socket creation error");
}

Socket::Socket(const Socket& s)
    : SocketBase(s),
      RemoteHost(this,&Socket::getRemoteHost,nullptr),
      RemotePort(this,&Socket::getRemotePort,nullptr)
{
    *this = s;
}

Socket::~Socket()
{
    Close();
}

Socket& Socket::operator=(const Socket& sock)
{
    // protected
    if (&sock != this) {
		SocketBase::operator=(sock);
		m_is_connected = sock.m_is_connected;
		m_remote_host = sock.m_remote_host;
		m_remote_port = sock.m_remote_port;
    }

    return *this;
}

void Socket::Connect(const char* host, unsigned short portn)
{
	// new way (2020)
	struct addrinfo hint, *servinfos, *info;
	memset(&hint, 0, sizeof hint);
	hint.ai_family = AF_INET;		// IPv4
	hint.ai_socktype = SOCK_STREAM;	// TCP

	int err = getaddrinfo(host, to_string(portn).c_str(), &hint, &servinfos);	// get list of servers
	throwif<ConnectionFailure>(err<0, "Connect::getaddrinfo()");

	// struct sockaddr_in servaddr;
	// memset((void*)&servaddr,0,sizeof(servaddr));
	// servaddr.sin_family = AF_INET;	// IPv4
	// servaddr.sin_port = htons(portn);
	// int err = inet_pton(AF_INET,host,&servaddr.sin_addr);
	// throwif<ConnectionFailure>(err<0,"inet_pton()");

	for (info = servinfos; info != nullptr; info = info->ai_next) {
		m_sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (m_sockfd != -1) break;	// first ok, get out!
	}
	throwif<ConnectionFailure>(info == nullptr, "Connect::socket()");

	// err = connect(m_sockfd,(const struct sockaddr*)&servaddr,sizeof(struct sockaddr));
	err = connect(m_sockfd, info->ai_addr, info->ai_addrlen);
	throwif<ConnectionFailure>(err<0,"Connect::connect()");

	freeaddrinfo(servinfos);

	m_is_connected = true;
	m_remote_host = host;
	m_remote_port = portn;
}

ssize_t Socket::Read(void* buf, const size_t count)
{
    return ::read(m_sockfd,buf,count);
}

ssize_t Socket::Write(const void* buf, const size_t count)
{
    return ::write(m_sockfd,buf,count);
}

ssize_t Socket::Receive(char* buf, const size_t len)
{
	ssize_t n = ::recv(m_sockfd,buf,len,0);
	throwif<ConnectionFailure>(n<0,"recv()");
	return n;
}

ssize_t Socket::Send(const char* buf, const size_t len)
{
	ssize_t total = 0;        // how many bytes we've sent
	ssize_t bytesleft = len ; // how many we have left to send
	ssize_t n;

	while(total < bytesleft) {
		n = ::send(m_sockfd, buf+total, bytesleft, 0);
		throwif<ConnectionFailure>(n<0,"::send()");
		total += n;
		bytesleft -= n;
	}
	return total;
}

template <class ANY>
void Socket::Send(const ANY& t)
{
	// type ANY must be sizable
	// warning: sending machine may have different byte order...
	ssize_t bytes = this->Send((char*)&t,sizeof(t));
	throwif<Disconnected>(bytes==0,"Send(ANY)");
}

template <class ANY>
void Socket::Receive(ANY& t)
{
	// type ANY must be sizable
	// warning: sending machine may have different byte order...
	ssize_t bytes = this->Receive((char*)&t,sizeof(t));
	throwif<Disconnected>(bytes==0,"Receive(ANY)");
}

Socket& Socket::operator<<(const string& msg)
{
	ssize_t sz = msg.size();
	auto bytes = this->Send(msg.c_str(),sz);
	throwif<Disconnected>(bytes==0,"operator<<(msg)");
	return *this;
}

Socket& Socket::operator>>(string& msg)
{
	msg.clear();
	const ssize_t BUFSZ = 4096;
	char* buf = new char[BUFSZ];
	auto bytes = this->Receive(buf,BUFSZ-1);
	buf[bytes] = '\0';
	throwif<Disconnected>(bytes==0,"operator<<(buf)");
	while (bytes == BUFSZ-1) {
		msg += buf;
		bytes = this->Receive(buf,BUFSZ-1);
		buf[bytes] = '\0';
		throwif<Disconnected>(bytes==0,"operator<<(buf)");
	}
	msg += buf;
	delete [] buf;
	return *this;
}

Socket& Socket::operator>>(int& x)
{
    Receive<int>(x);
    return *this;
}

Socket& Socket::operator<<(const int x)
{
    Send<int>(x);
    return *this;
}

Socket& Socket::operator>>(float& x)
{
    Receive<float>(x);
    return *this;
}

Socket& Socket::operator<<(const float x)
{
    Send<float>(x);
    return *this;
}

void Socket::Close()
{
	SocketBase::Close();
	m_is_connected = false;
}

const char* Socket::getRemoteHost() const
{
	if (!IsConnected())
		return nullptr;
	return m_remote_host.c_str();
}

unsigned short Socket::getRemotePort() const
{
	if (!IsConnected())
		return 0;
	return m_remote_port;
}

// -------------------------------------------------------
// ServerSocket
// -------------------------------------------------------

ServerSocket::ServerSocket()
    : Port(this,&ServerSocket::getPort,nullptr)
{
    init(0);	// portn == 0 makes kernel choose available port
}

ServerSocket::ServerSocket(unsigned short portn)
    : Port(this,&ServerSocket::getPort,nullptr)
{
    init(portn);
}

ServerSocket::ServerSocket(const string& ip, Port_t portn)
	: Port(this,&ServerSocket::getPort,nullptr)
{
	init(portn,ip);
}

ServerSocket::ServerSocket(const ServerSocket& ss)
    : SocketBase(ss), Port(this,&ServerSocket::getPort,nullptr)
{
    *this = ss;
}

ServerSocket::~ServerSocket()
{
    Close();
}

void ServerSocket::init(unsigned short portn, const string& ip)
{
	m_type = SocketType::SERVER_SOCK;

	struct addrinfo hint, *servinfos;

	memset(&hint, 0, sizeof hint);
	hint.ai_family = AF_INET;			// IPv4
	hint.ai_socktype = SOCK_STREAM;		// TCP
	hint.ai_flags = AI_PASSIVE;			// Use address return by getaddrinfo in bind() return addrinfo will have INADDR_ANY set to IPv4 adress and INADDR6_ANY_INIT for IPv6 address (discarded if node != nullptr)

	// get address list
	const char* node = ip.empty() ? nullptr : ip.c_str();
	int err = getaddrinfo(node, to_string(portn).c_str(), &hint, &servinfos);
	throwif<Socket::ConnectionFailure>(err != 0, "init::getaddrinfo");

	struct addrinfo* info;
	for (info = servinfos; info != nullptr; info = info->ai_next) {
		m_sockfd = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if (m_sockfd != -1) break;	// first that is ok is ok
	}
	throwif<Socket::ConnectionFailure>(m_sockfd == -1,"ServerSocket::init::socket()");

	// m_sockfd = ::socket(AF_INET,SOCK_STREAM,0);
	// throwif<Socket::ConnectionFailure>(m_sockfd == -1,"ServerSocket::init::socket()");

	// struct sockaddr_in servaddr;
	// socklen_t SLEN = sizeof(servaddr);	// must not be a constant
	// memset(&servaddr,0,SLEN);
	// servaddr.sin_family = AF_INET;
	// servaddr.sin_port = htons(portn);
	// if (ip.empty())
	// 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // accepts client connection from any network interface
	// else
	// 	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

	err = ::bind(m_sockfd, info->ai_addr, info->ai_addrlen);
	throwif<Socket::ConnectionFailure>(err==-1,"ServerSocket::init::bind()");

	// get the current address for the specified socket (with field updates)
	// err = getsockname(m_sockfd,(struct sockaddr*)&servaddr,&SLEN);
	err = getsockname(m_sockfd, info->ai_addr, &info->ai_addrlen);
	throwif<Socket::ConnectionFailure>(err==-1,"ServerSocket::init::getsockname()");

	// get actual port no.
	// assumes info->ai_familiy == AF_INET
	m_portn = static_cast<unsigned short>(ntohs(reinterpret_cast<struct sockaddr_in*>(info->ai_addr)->sin_port));

	// write on screen chosen local host
	//    struct in_addr myaddr;
	//    memset(&myaddr,0,sizeof(myaddr));
	//    myaddr.s_addr = servaddr.sin_addr.s_addr;
	//    cout << inet_ntoa(myaddr) << endl;

	err = listen(m_sockfd, LISTENQUEUE);
	throwif<Socket::ConnectionFailure>(err>0,"ServerSocket::init::listen()");
}

ServerSocket& ServerSocket::operator=(const ServerSocket& ss)
{
    if (&ss != this) {
		SocketBase::operator=(ss);
		m_portn = ss.m_portn;
    }
    return *this;
}

Socket* ServerSocket::Accept()
{
	// User is responsible for releasing memory
	struct sockaddr_in remote_addr;
	socklen_t addrlen = sizeof(remote_addr);
	int fd = ::accept(m_sockfd, (struct sockaddr*)&remote_addr, &addrlen);
	throwif<Socket::ConnectionFailure>(fd<0,"ServerSocket::Accept::accept()");
	Socket* sock = new Socket(fd);
	sock->m_remote_host = inet_ntoa(remote_addr.sin_addr);
	sock->m_remote_port = remote_addr.sin_port;
	sock->m_is_connected = true;
	return sock;
}

// -------------------------------------------------------
// SocketSelector
// -------------------------------------------------------

SocketSelector::SocketSelector()
    : m_maxfd(0)
{
	FD_ZERO(&m_master);
	FD_ZERO(&m_readfs);
}

SocketSelector::~SocketSelector()
{
	Clear();
}

void SocketSelector::Add(SocketBase* sock)
{
	int fd = sock->FD;
	m_socks[fd] = sock;
	if (!FD_ISSET(fd,&m_master)) {
		FD_SET(fd,&m_master);
		UpdateMaxFD();
	}
}

void SocketSelector::Remove(SocketBase* sock)
{
	int fd = sock->FD;
	if (FD_ISSET(fd,&m_master)) {
		delete m_socks[fd];
		m_socks.erase(fd);
		FD_CLR(fd,&m_master);
		UpdateMaxFD();
	}
}

void SocketSelector::UpdateMaxFD()
{
	map<int,SocketBase*>::iterator it = m_socks.end();
	m_maxfd = 0;

	// is map sorted? get the last
	for (it = m_socks.begin(); it != m_socks.end(); it++)
		if (it->first > m_maxfd)
			m_maxfd = it->first;
}

void SocketSelector::Clear()
{
	FD_ZERO(&m_master);
	map<int,SocketBase*>::iterator it;
	for (it = m_socks.begin(); it != m_socks.end(); it++)
		delete it->second;
	m_socks.clear();
}

bool SocketSelector::IsIn(const SocketBase* sock)
{
	return (bool)(FD_ISSET(sock->FD,&m_master));
}

int SocketSelector::ReadSelect(SockVec* socks, struct timeval* timeout)
{
	int res;
	m_readfs = m_master;
	res = ::select(m_maxfd+1,&m_readfs,nullptr,nullptr,timeout);
	throwif<Socket::ConnectionFailure>(res==-1,"ReadSelect");

	map<int,SocketBase*>::iterator it;
	socks->clear();
	for (it = m_socks.begin(); it != m_socks.end(); it++)
		if (FD_ISSET(it->first,&m_readfs))
			socks->push_back(it->second);
	return res;
}

// --------------------------------------------------
// UDP Sockets
// --------------------------------------------------

USocket::USocket()
{
	m_usockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

ssize_t USocket::Send(const char* buf, const size_t len,
	const std::string& ip, Port_t port)
{
	struct sockaddr_in servaddr;
	const socklen_t SZ = sizeof(struct sockaddr_in);
	memset(&servaddr,0,SZ);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	int error = inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
	throwif<Socket::ConnectionFailure>(error == INADDR_NONE,"Malformed address: inet_pton()");

	auto n = sendto(m_usockfd, buf, len, 0, (struct sockaddr*)&servaddr,SZ);
	throwif<Socket::ConnectionFailure>(n!=len,"sendto(): sent less bytes");
	return n;
}

// ---------------------------------------------------

UServerSocket::UServerSocket(Port_t port)
	: m_usockfd{-1}, m_origin_port{0}
{
	init(port);
}

UServerSocket::UServerSocket(const std::string& ip, Port_t port)
	: m_usockfd{-1}, m_origin_port{0}
{
	init(port,ip);
}

void UServerSocket::init(Port_t port, const std::string& ip)
{
	m_usockfd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in servaddr;
	const socklen_t SZ = sizeof(struct sockaddr_in);
	memset(&servaddr,0,SZ);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = ip.empty() ? htonl(INADDR_ANY) : inet_addr(ip.c_str());
	servaddr.sin_port = htons(port);

	unsigned int yes = 1;
	//setsockopt(m_usockfd,SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(yes));

	int err = ::bind(m_usockfd,(struct sockaddr*)&servaddr,SZ);
	throw_if(err==-1,"UServerSocket::init::bind()");

	char abuf[INET_ADDRSTRLEN];
	cout << inet_ntop(AF_INET,&servaddr.sin_addr,abuf,sizeof(abuf))
		<< ":" << servaddr.sin_port << endl;
}

ssize_t UServerSocket::Receive(char* buf, const size_t len)
{
	struct sockaddr_in cliaddr;
	socklen_t alen = sizeof(struct sockaddr_in);
	ssize_t n = recvfrom(m_usockfd,buf,len,0,(struct sockaddr*)&cliaddr,&alen);
	char abuf[INET_ADDRSTRLEN];
	m_origin_ip = inet_ntop(AF_INET,&cliaddr.sin_addr,abuf,sizeof(abuf));
	m_origin_port = cliaddr.sin_port;
	return n;
}
