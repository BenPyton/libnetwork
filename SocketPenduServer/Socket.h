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

#include <iostream>
#include <Winsock2.h>

using namespace std;

class Socket
{
private:
	SOCKET m_socket;
	sockaddr_in m_sin;

public:
	enum Protocol { TCP, UDP };

	Socket(string _addr, unsigned short _port, Protocol _p);
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