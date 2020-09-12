//
// SOCKET LIBRARY
//
//   (C) 2005-2020 LAPLJ.
//   All rights reserved.
//   PPGIa - PUCPR
//
// OS: UNIX
//

#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include "Property.h"
#include <stdexcept>

namespace colibry {

	enum class SocketType { UNDEFINED, CLIENT_SOCK, SERVER_SOCK };
	using Port_t = unsigned short;

	//
	// SocketBase
	//

	class SocketBase {
	public:

		virtual ~SocketBase();
		virtual SocketBase& operator=(const SocketBase& sock);
		virtual bool operator==(const SocketBase& sock);
		virtual void Close();

		Property<SocketBase,int,'r'> FD;
		Property<SocketBase,SocketType,'r'> Type;

	protected:

		SocketBase(); // this class cannot be directly instantiated
		SocketBase(const SocketBase& sock);

		// Getters
		virtual int getFD() const { return m_sockfd; }
		virtual SocketType getType() const { return m_type; }

		int m_sockfd;
		SocketType m_type;
	};

	class ServerSocket;

	// --------------------------------------------------
	// (Client) TCP Socket
	// --------------------------------------------------

	class Socket : public SocketBase {
	public:

		// Exceptions

		class Disconnected : public std::runtime_error {
		public:
			Disconnected() : std::runtime_error{"disconnected"} {}
			Disconnected(const std::string& foo) : std::runtime_error{"disconnected"} {}
		};

		class ConnectionFailure : public std::runtime_error {
		public:
			ConnectionFailure(const std::string& details) : std::runtime_error(details) {}
		};

	public:

		explicit Socket();              // create new socket
		explicit Socket(const int fd);  // instantiates socket with provided fd
		virtual ~Socket();				// close socket + free memory

		// Connect to listening (server) socket
		virtual void Connect(const char* host, Port_t port);

		// Raw data exchange
		virtual ssize_t Read(void* buf, const size_t len);
		virtual ssize_t Write(const void* buf, const size_t len);

		// Raw data exchange (same as Read and Write - prefer this)
		// may throw ConnectionFailure exceptions
		virtual ssize_t Receive(char* buf, const size_t len);
		virtual ssize_t Send(const char* buf, const size_t len);

		// may throw ConnectionFailure and Disconnected exceptions
		virtual Socket& operator<<(const std::string& s);
		virtual Socket& operator>>(std::string& s);
		// for basic types
		template <class ANY> void Send(const ANY& x);
		template <class ANY> void Receive(ANY& x);
		// Integers
		virtual Socket& operator<<(const int x);
		virtual Socket& operator>>(int& x);
		// Floats
		virtual Socket& operator<<(const float x);
		virtual Socket& operator>>(float& x);

		virtual void Close();

		// Check whether socket is connected
		bool IsConnected() const { return m_is_connected; }

		// Properties
		Property<Socket,const char*,'r'> RemoteHost;
		Property<Socket,Port_t,'r'> RemotePort;

		friend class ServerSocket;

	protected:

		// The first to go out of scope will close socket.
		// So it's not a good idea to copy sockets
		Socket(const Socket& sock);
		Socket& operator=(const Socket& sock);

		// Get Remote Host/Port name if connected
		const char* getRemoteHost() const;
		Port_t getRemotePort() const;

	protected:

		bool m_is_connected;
		std::string m_remote_host;
		Port_t m_remote_port;
	};

	//
	// SockVec
	//

	using SockVec = std::vector<SocketBase*>;

	// --------------------------------------------------
	// ServerSocket
	// --------------------------------------------------

	class ServerSocket : public SocketBase {
	public:

		explicit ServerSocket();				// let the kernel choose available port number
		explicit ServerSocket(Port_t portn);   // Create socket listening on portn
		explicit ServerSocket(const std::string& ip, Port_t portn);
		explicit ServerSocket(const ServerSocket& ss);
		virtual ~ServerSocket();

		ServerSocket& operator=(const ServerSocket& ss);
		// Wait for connection returning pointer to connected client socket
		virtual Socket* Accept();  // user is responsible for releasing memory

		Property<ServerSocket,Port_t,'r'> Port;

	protected:

		Port_t getPort() const { return m_portn; }

		void init(Port_t portn, const std::string& ip = "");	// default for ip = localhost
		Port_t m_portn;
	};

	// --------------------------------------------------
	// SocketSelector (wrapper to select command)
	// --------------------------------------------------

	class SocketSelector {
	public:

		SocketSelector();
		virtual ~SocketSelector();

		// Add/remove socket to/from the list of managed sockets
		void Add(SocketBase* sock);    // SocketSelector is owner of sock
		void Remove(SocketBase* sock); // Releases socket memory
		// Empty list of managed sockets
		void Clear();
		// Check is sock is managed by this selector
		bool IsIn(const SocketBase* sock);

		// Wait for events in monitored sockets (see manpages select)
		int ReadSelect(SockVec* socks, struct timeval *timeout=nullptr);

	protected:

		void UpdateMaxFD();

	protected:

		fd_set m_master;
		fd_set m_readfs;
		unsigned short m_maxfd;
		std::map<int,SocketBase*> m_socks;
	};

	// --------------------------------------------------
	// UDP Sockets
	// --------------------------------------------------

	class USocket {
	public:
		explicit USocket();
		virtual ssize_t Send(const char* buf, const size_t len,
			const std::string& ip, Port_t port);
	protected:
		int m_usockfd;
	};

	class UServerSocket {
	public:
		explicit UServerSocket(Port_t port);
		explicit UServerSocket(const std::string& ip, Port_t port);

		virtual ssize_t Receive(char* buf, const size_t len);

		int getFD() const { return m_usockfd; }
		Port_t getOriginPort() const { return m_origin_port; }
		std::string getOriginIP() const { return m_origin_ip; }

	protected:
		void init(Port_t port, const std::string& ip="");
	protected:
		int m_usockfd;
		Port_t m_port;
		Port_t m_origin_port;
		std::string m_origin_ip;
	};

}		// namespace colibry

#endif
