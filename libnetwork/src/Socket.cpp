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

#include "Network/Socket.h"
#include <Ws2tcpip.h>
#include "Utils.h"
#include "Network/SocketException.h"

namespace net
{
	Socket::Socket(std::string _addr, unsigned short _port, Protocol _p)
		: m_protocol(_p)
	{
		// AF_INET internet address ipv4
		// AF_INET6 ipv6
		// SOCK_STREAM with TCP for TCP protocol
		// SOCK_DGRAM with UDP for UDP protocol
		switch (_p)
		{
		case Protocol::TCP:
			m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			inet_pton(AF_INET, _addr.c_str(), &m_sin.sin_addr);
			break;
		case Protocol::UDP:
			m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			m_sin.sin_addr.s_addr = INADDR_ANY;
			break;
		default: break;
		}

		if (m_socket == INVALID_SOCKET)
		{
			_PrintErrorAndExit("Error when creating socket");
		}

		m_sin.sin_family = AF_INET;
		m_sin.sin_port = htons(_port);
	}

	Socket::Socket(Socket && _s)
	{
		std::swap(m_protocol, _s.m_protocol);
		std::swap(m_socket, _s.m_socket);
		std::swap(m_sin, _s.m_sin);
	}


	Socket::Socket(SOCKET _sock, sockaddr_in _sin, Protocol _p)
		: m_socket(_sock), m_sin(_sin), m_protocol(_p)
	{
		//memset(&m_sin, 0, sizeof(m_sin));
	}

	Socket::~Socket()
	{
		Close();
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
		if (m_protocol == Protocol::TCP && listen(m_socket, 0) < 0)
		{
			_PrintErrorAndExit("Can't listen on socket");
		}
	}

	Socket Socket::Accept()
	{
		if (m_protocol != Protocol::TCP)
		{
			_PrintErrorAndExit("Socket is not in TCP Protocol");
			return Socket(NULL, NULL, Protocol::UDP);
		}

		sockaddr_in clientAddr;
		int len = sizeof(clientAddr);

		SOCKET s = accept(m_socket, (SOCKADDR*)&clientAddr, &len);

		if (s == INVALID_SOCKET)
		{
			closesocket(s);
			_PrintErrorAndExit("Error when accepting client");
		}
		return Socket(s, clientAddr, Protocol::TCP);
	}

	bool Socket::Connect()
	{
		bool success = true;
		if (m_protocol != Protocol::TCP)
		{
			_PrintErrorAndExit("Socket is not in TCP Protocol");
			success = false;
		}
		else if (connect(m_socket, (SOCKADDR*)&m_sin, sizeof(m_sin)) < 0)
		{
			_PrintErrorAndExit("Can't connect");
			success = false;
		}
		return success;
	}

	int Socket::Send(const char * _buf, int _len)
	{
		int result = -1;
		if (m_protocol != Protocol::TCP)
		{
			_PrintErrorAndExit("Socket is not in TCP Protocol");
		}
		else if ((result = send(m_socket, _buf, _len, 0)) == SOCKET_ERROR)
		{
			_PrintErrorAndExit("Error when sending data");
		}
		return result;
	}

	int Socket::Recv(char * _buf, int _len)
	{
		int result = -1;
		if (m_protocol != Protocol::TCP)
		{
			_PrintErrorAndExit("Socket is not in TCP Protocol");
		}
		else if ((result = recv(m_socket, _buf, _len, 0)) == SOCKET_ERROR)
		{
			_PrintErrorAndExit("Error when recieving data");
		}
		return result;
	}

	int Socket::SendTo(const char * _buf, int _len, string _dst_addr, unsigned short _dst_port)
	{
		int result = -1;
		if (m_protocol != Protocol::UDP)
		{
			_PrintErrorAndExit("Socket is not in UDP Protocol");
		}
		else
		{
			sockaddr_in dst;
			dst.sin_family = AF_INET;
			dst.sin_port = htons(_dst_port);
			inet_pton(AF_INET, _dst_addr.c_str(), &dst.sin_addr);
			if ((result = sendto(m_socket, _buf, _len, 0, (SOCKADDR*)&dst, sizeof(dst))) == SOCKET_ERROR)
			{
				_PrintErrorAndExit("Error when sending data");
			}
		}
		return result;
	}

	int Socket::RecvFrom(char * _buf, int _len, string& _src_addr, unsigned short& _src_port)
	{
		int result = -1;
		if (m_protocol != Protocol::UDP)
		{
			_PrintErrorAndExit("Socket is not in UDP Protocol");
		}
		else
		{
			sockaddr_in src;
			socklen_t srclen = sizeof(src);
			if ((result = recvfrom(m_socket, _buf, _len, 0, (SOCKADDR*)&src, &srclen)) == SOCKET_ERROR)
			{
				_PrintErrorAndExit("Error when recieving data");
			}
			else
			{
				char bufAddr[INET6_ADDRSTRLEN];
				if (inet_ntop(src.sin_family, &src.sin_addr, (PSTR)bufAddr, INET6_ADDRSTRLEN) == NULL)
				{
					_PrintErrorAndExit("Error when retreiving src address");
				}
				else
				{
					_src_addr.clear();
					_src_addr.append(bufAddr);
					_src_port = ntohs(src.sin_port);
				}
			}
		}
		return result;
	}

	void Socket::Close()
	{
		closesocket(m_socket);
	}

	string Socket::GetIP()
	{
		if (m_cachedIP == "")
		{
			UINT16 tmp;
			_GetAddrStrAndPort(&m_sin, m_cachedIP, tmp);
		}
		return m_cachedIP;
	}

	UINT16 Socket::GetPort()
	{
		return ntohs(m_sin.sin_port);
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

	string Socket::GetHostName()
	{
		char bufHost[255];
		memset(bufHost, 0, 255);
		if (gethostname(bufHost, 255) != 0)
		{
			_PrintErrorAndExit("Error when getting host name");
		}
		return string(bufHost);
	}

	vector<string> Socket::GetLocalIP(string _nodeName, string _serviceName, Protocol _protocol, IPVersion _ipversion)
	{
		addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = _ipversion == IPVersion::IPV4 ? AF_INET : AF_INET6;
		hints.ai_socktype = _protocol == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM;

		const char* node = _nodeName.size() > 0 ? _nodeName.c_str() : nullptr;
		const char* service = _serviceName.size() > 0 ? _serviceName.c_str() : nullptr;

		addrinfo* infos = nullptr;
		if (getaddrinfo(node, service, &hints, &infos) != 0)
		{
			_PrintErrorAndExit("Error when getting address info");
		}

		vector<string> ipList;
		UINT16 port;
		string addr;
		for (addrinfo* pInfo = infos; pInfo != nullptr; pInfo = pInfo->ai_next)
		{
			_GetAddrStrAndPort((sockaddr_in*)pInfo->ai_addr, addr, port);
			ipList.push_back(addr);
		}

		freeaddrinfo(infos);

		return ipList;
	}

	Socket & Socket::operator=(Socket && _s)
	{
		std::swap(m_protocol, _s.m_protocol);
		std::swap(m_socket, _s.m_socket);
		std::swap(m_sin, _s.m_sin);
		return *this;
	}


	void Socket::_PrintErrorAndExit(string _msg)
	{
		int err = WSAGetLastError();
		DebugLog("[Error] %s: %d\n", _msg.c_str(), err);
		throw SocketException(_msg.c_str(), err);
	}

	void Socket::_GetAddrStrAndPort(sockaddr_in* _addr, string& _str, UINT16& _port)
	{
		char bufAddr[INET6_ADDRSTRLEN];
		memset(bufAddr, 0, INET6_ADDRSTRLEN);
		if (inet_ntop(_addr->sin_family, &_addr->sin_addr, (PSTR)bufAddr, INET6_ADDRSTRLEN) == NULL)
		{
			_PrintErrorAndExit("Error when retreiving address and port");
		}
		else
		{
			_str = string(bufAddr);
			_port = ntohs(_addr->sin_port);
		}
	}
}
