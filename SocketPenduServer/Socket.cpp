/*
* @author PELLETIER Benoit
*
* @file Socket.cpp
*
* @date 26/10/2018
*
* @brief Windows socket class
*
*/

#include "stdafx.h"
#include "Socket.h"

Socket::Socket(string _addr, unsigned short _port, Protocol _p)
{
	switch (_p)
	{
	case Protocol::TCP:
		// af_inet internet address
		// stream with TCP for TCP protocol
		// datagram with UDP for UDP protocol
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		break;
	case Protocol::UDP:
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;
	default: break;
	}

	if (m_socket == INVALID_SOCKET)
	{
		_PrintErrorAndExit("Error when creating socket");
	}


	m_sin.sin_family = AF_INET;
	m_sin.sin_addr.s_addr = inet_addr(_addr.c_str());
	m_sin.sin_port = htons(_port);
}

Socket::Socket(const Socket & _s)
	: m_socket(_s.m_socket)
{
	memcpy(&m_sin, &_s.m_sin, sizeof(m_sin));
}


Socket::Socket(SOCKET _sock)
	: m_socket(_sock)
{
	memset(&m_sin, 0, sizeof(m_sin));
}

Socket::~Socket()
{
	//Close();
}

void Socket::Bind()
{
	if (bind(m_socket, (SOCKADDR*)&m_sin, sizeof(m_sin)) < 0)
	{
		_PrintErrorAndExit("Can't bind socket");
	}
}

void Socket::Listen()
{
	if (listen(m_socket, 0) < 0)
	{
		_PrintErrorAndExit("Can't listen on socket");
	}
}

Socket Socket::Accept()
{
	SOCKET s = accept(m_socket, nullptr, nullptr);
	if (s == INVALID_SOCKET)
	{
		closesocket(s);
		_PrintErrorAndExit("Error when accepting client");
	}
	return Socket(s);
}

void Socket::Connect()
{
	if (connect(m_socket, (SOCKADDR*)&m_sin, sizeof(m_sin)) < 0)
	{
		_PrintErrorAndExit("Can't connect");
	}
}

void Socket::Send(const char * _buf, int _len)
{
	if (send(m_socket, _buf, _len, 0) == SOCKET_ERROR)
	{
		_PrintErrorAndExit("Error when sending data");
	}
}

void Socket::Recv(char * _buf, int _len)
{
	if (recv(m_socket, _buf, _len, 0) == SOCKET_ERROR)
	{
		_PrintErrorAndExit("Error when recieving data");
	}
}

void Socket::Close()
{
	closesocket(m_socket);
}

bool Socket::Init()
{
	WSAData data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		WSACleanup();
		return false;
	}
	return true;
}

void Socket::Quit()
{
	WSACleanup();
}

Socket & Socket::operator=(const Socket & _s)
{
	if (this != &_s);
	{
		m_socket = _s.m_socket;
		memcpy(&m_sin, &_s.m_sin, sizeof(m_sin));
	}
	return *this;
}

void Socket::_PrintErrorAndExit(string _msg)
{
	int err = WSAGetLastError();
	printf("[Error] %s: %d\n", _msg.c_str(), err);
	throw exception(_msg.c_str(), err);
	//system("pause"); // allow reading of error message
	//Quit();
	//exit(errno);
}
