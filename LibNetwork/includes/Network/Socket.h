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

//#include <iostream>
#include <Winsock2.h>
#include "Exports.h"
#include <string>

using namespace std;

class LIBNETWORK_API Socket
{
private:
	SOCKET m_socket;
	sockaddr_in m_sin;

public:
	enum Protocol { TCP, UDP };

	Socket(wstring _addr, unsigned short _port, Protocol _p);
	Socket(const Socket& _s);
	~Socket();

	void Bind();
	void Listen();
	Socket Accept();
	void Connect();
	void Send(const char* _buf, int _len);
	void Recv(char* _buf, int _len);
	void Close();

	static bool Init();
	static void Quit();

	Socket& operator=(const Socket& _s);
private:
	Socket(SOCKET _sock);
	static void _PrintErrorAndExit(string _msg);
};

#endif // _SOCKET_H