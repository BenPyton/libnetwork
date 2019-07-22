/*
* @author PELLETIER Benoit
*
* @file ClientV2.cpp
*
* @date 18/07/2019
*
* @brief Simple client class
*
*/

#include "Network/ClientV2.h"
#include "Network/Socket.h"
#include "Network/SocketException.h"
#include "Utils.h"

namespace net{
	ClientV2::ClientV2()
		: MsgSystem()
	{
	}

	ClientV2::~ClientV2()
	{
		if (m_running)
		{
			disconnect();
		}
	}

	void ClientV2::connect(string _addr, unsigned short _port)
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

	void ClientV2::disconnect()
	{
		if (!m_running)
		{
			throw SocketException("Can't disconnect client when not connected.", 0);
		}

		m_running = false;

		WaitEndOfThreads();
		CloseAllSockets();
	}

	void ClientV2::send(string _msg)
	{
		if (m_running)
		{
			AddMsgToSendQueue(m_sockets.front(), _msg);
		}
	}
}
