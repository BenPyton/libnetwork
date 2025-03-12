/*
* @author PELLETIER Benoit
*
* @file Socket.h
*
* @date 26/10/2018
*
* @brief Windows socket class
*
*/

#ifndef _SOCKET_H
#define _SOCKET_H

#include <Winsock2.h>
#include "Exports.h"
#include <string>
#include <vector>
#include "Utils.h"

//#pragma warning(push)
//#pragma warning(disable:4251)

// https://broux.developpez.com/articles/c/sockets/

using namespace std;

namespace net
{

	enum class Protocol { TCP, UDP };
	enum class IPVersion { IPV4, IPV6 };

	class LIBNETWORK_API Socket
	{
	private:
		SOCKET m_socket;
		sockaddr_in m_sin;
		Protocol m_protocol;

		string m_cachedIP;

	public:

		Socket(string _addr, unsigned short _port, Protocol _p);
		Socket(const Socket& _s) = delete; // no copy constructor
		Socket(Socket&& _s); // move contructor
		~Socket();

		void Bind();
		void Listen();
		Socket Accept();
		bool Connect();
		int Send(const char* _buf, int _len);
		int Recv(char* _buf, int _len);
		int SendTo(const char* _buf, int _len, string _dst_addr, unsigned short _dst_port);
		int RecvFrom(char* _buf, int _len, string& _src_addr, unsigned short& _src_port);
		void Close();

		string GetIP();
		UINT16 GetPort();
		SOCKET GetSock() { return m_socket; }

		static bool Init();
		static void Quit();
		static string GetHostName();
		static vector<string> GetLocalIP(string _nodeName, string _serviceName, Protocol _protocol = Protocol::TCP, IPVersion _ipversion = IPVersion::IPV4);

		Socket& operator=(Socket&& _s);
		Socket& operator=(const Socket& _s) = delete;
	private:
		Socket(SOCKET _sock, sockaddr_in _addr, Protocol _p);
		static void _PrintErrorAndExit(string _msg);
		static void _GetAddrStrAndPort(sockaddr_in* _addr, string& _str, UINT16& _port);
	};
}

//#pragma warning(pop)

#endif // _SOCKET_H