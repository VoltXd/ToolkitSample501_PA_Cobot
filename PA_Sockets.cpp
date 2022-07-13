#include "StdAfx.h"
#include "PA_Sockets.hpp"

#include <iostream>

namespace PA_Communication
{
	bool Start()
	{
#ifdef _WIN32
		WSAData wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
		return true;
#endif
	}

	void Release()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}

	int GetError()
	{
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	void CloseSocket(SOCKET s)
	{
#ifdef _WIN32
		closesocket(s);
#else
		close(s);
#endif
	}

	UdpSocketManager::UdpSocketManager(const char* localIPAddress, unsigned short localPort, const char* remoteIPAddress, unsigned short remotePort)
	{
		// Retrieve Local values
		mLocalIpAddress = std::string(localIPAddress);
		mLocalPort = localPort;

		// Retrieve Remote values
		mRemoteIpAddress = std::string(remoteIPAddress);
		mRemotePort = remotePort;
	}

	UdpSocketManager::UdpSocketManager(unsigned short localPort, const char* remoteIPAddress, unsigned short remotePort)
	{
		// Retrieve Local values
		mLocalPort = localPort;

		// Retrieve Remote values
		mRemoteIpAddress = std::string(remoteIPAddress);
		mRemotePort = remotePort;
	}

	UdpSocketManager::~UdpSocketManager()
	{
		Close();
	}

	void UdpSocketManager::Initialize()
	{
		mIsInitialized = true;
		// Initialize use of WinSock DLL
		if (!PA_Communication::Start())
		{
			SocketError(UdpSocketError::WinSockError);
			mIsInitialized = false;
		}

		// Create socket
		mUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (mUdpSocket == SOCKET_ERROR)
		{
			SocketError(UdpSocketError::SocketCreationError);
			mIsInitialized = false;
		}

		// Saving local address
		if (mLocalIpAddress.empty())
			mLocalAddr.sin_addr.s_addr = INADDR_ANY;
		else
			inet_pton(AF_INET, mLocalIpAddress.data(), &mLocalAddr.sin_addr.s_addr);
		mLocalAddr.sin_port = htons(mLocalPort);
		mLocalAddr.sin_family = AF_INET;

		// Bind the socket to the used local adress and port (Allows to listen and send from this address and port)
		if (bind(mUdpSocket, reinterpret_cast<sockaddr*>(&mLocalAddr), sizeof(mLocalAddr)) != 0)
		{
			SocketError(UdpSocketError::BindError);
			mIsInitialized = false;
		}

		// Saving remote address
		inet_pton(AF_INET, mRemoteIpAddress.data(), &mRemoteAddr.sin_addr.s_addr);
		mRemoteAddr.sin_family = AF_INET;
		mRemoteAddr.sin_port = htons(mRemotePort);
	}

	void UdpSocketManager::Close()
	{
		PA_Communication::CloseSocket(mUdpSocket);
		PA_Communication::Release();
	}

	bool UdpSocketManager::IsInitialized()
	{
		return mIsInitialized;
	}

	int UdpSocketManager::Send(const char* msg)
	{
		// Send the message to the remote address
		int totalBytesSent = sendto(mUdpSocket, msg, strlen(msg), 0, reinterpret_cast<const sockaddr*>(&mRemoteAddr), sizeof(mRemoteAddr));
		
		// Verify error
		if (totalBytesSent <= 0)
			SocketError(UdpSocketError::SendError);
		return totalBytesSent;
	}

	int UdpSocketManager::Receive(char *msg, sockaddr_in& from)
	{
		// Receive a message from any device writing on Local Port 
		int fromlen = sizeof(from);
		char msgCpy[1500] = { 0 };
		int totalBytesReceived = recvfrom(mUdpSocket, msgCpy, sizeof(msgCpy) - 1, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
		if (totalBytesReceived <= 0)
			SocketError(UdpSocketError::ReceiveError);
		else
			for (int i = 0; i < totalBytesReceived; i++)
				msg[i] = msgCpy[i];
			
		return totalBytesReceived;
	}
	
	void UdpSocketManager::SocketError(UdpSocketError sockError)
	{
		// Retrieve the socket error type
		std::string errorString;
		switch (sockError)
		{
		case WinSockError:
			errorString.append("WinSock Initialization");
			break;

		case SocketCreationError:
			errorString.append("Socket Creation");
			break;

		case BindError:
			errorString.append("Socket Bind");
			break;

		case SendError:
			errorString.append("Data Transmission");
			break;

		case ReceiveError:
			errorString.append("Data Reception");
			break;

		default:
			errorString.append("Unknown");
			break;
		}

		// Code to run with a terminal program
		std::cout << errorString << " error : " << WSAGetLastError() << std::endl << std::endl;
	}
}