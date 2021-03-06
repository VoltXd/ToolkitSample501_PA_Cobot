#pragma once

#include <string>

#ifdef _WIN32
#if _MSC_VER >= 1800
#include <WS2tcpip.h>
#else
#define inet_pton(FAMILY, IP, PTR_STRUCT_SOCKADDR) (*(PTR_STRUCT_SOCKADDR)) = inet_addr((IP))
typedef int socklen_t;
#endif

#include <WinSock2.h>

#ifdef _MSC_VER
#if _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
//!< Win8.1 & higher
#pragma comment(lib, "Ws2_32.lib")
#else
#pragma comment(lib, "wsock32.lib")
#endif

#endif

#else
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <unistd.h>  // close
#include <cerrno> // errno
#define SOCKET int
#define INVALID_SOCKET ((int)-1)
#endif

namespace PA_Communication
{
	bool Start();
	void Release();
	int GetError();
	void CloseSocket(SOCKET socket);
	
	enum UdpSocketError
	{
		WinSockError,
		SocketCreationError,
		BindError,
		SendError,
		ReceiveError
	};

	class UdpSocketManager
	{
	public:
		UdpSocketManager(const char* localIpAddress, unsigned short localPort, const char* remoteIpAddress, unsigned short remotePort);
		UdpSocketManager(unsigned short localPort, const char* remoteIpAddress, unsigned short remotePort);
		~UdpSocketManager();

		void Initialize();
		void Close();
		bool IsInitialized();
		int Send(const char *msg);
		int Receive(char *msg, sockaddr_in& from);
				

	private:
		void SocketError(UdpSocketError sockError);

		bool mIsInitialized = false;
		SOCKET mUdpSocket;

		std::string		mLocalIpAddress;
		unsigned short	mLocalPort;
		sockaddr_in		mLocalAddr;

		std::string		mRemoteIpAddress;
		unsigned short	mRemotePort;
		sockaddr_in		mRemoteAddr;
	};
}