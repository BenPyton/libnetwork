/*
 * @author PELLETIER Benoit
 *
 * @file Client.cpp
 *
 * @date 18/07/2019
 *
 * @brief Simple client class
 *
 */

#include "Network/Client.h"
#include "Network/Socket.h"
#include "Network/SocketException.h"
#include "Utils.h"

namespace net
{
	Client::Client()
		: MsgSystem()
	{
	}

	Client::~Client()
	{
		if (m_running)
		{
			disconnect();
		}
	}

	void Client::connect(std::string _addr, unsigned short _port)
	{
		if (m_running)
		{
			throw SocketException("Can't connect client when already connected.", 0);
		}

		Socket* socket = new Socket(_addr, _port, Protocol::TCP);
		socket->Connect();
		DebugLog("Successfully connected to server.\n");

		AddSocket(socket);

		m_running = true;
		RunThreads();
	}

	void Client::disconnect()
	{
		if (!m_running)
		{
			throw SocketException("Can't disconnect client when not connected.", 0);
		}

		m_running = false;

		WaitEndOfThreads();
		CloseAllSockets();
	}

	void Client::send(std::string _msg)
	{
		if (m_running)
		{
			AddMsgToSendQueue(m_sockets.front(), _msg);
		}
	}
} //namespace net
