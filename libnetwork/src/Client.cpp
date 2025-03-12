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
		if (isRunning())
		{
			disconnect();
		}
	}

	void Client::connect(std::string _addr, unsigned short _port)
	{
		if (isRunning())
		{
			throw SocketException("Can't connect client when already connected.", 0);
		}

		m_pClient = new Socket(_addr, _port, Protocol::TCP);
		m_pClient->Connect();
		DebugLog("Successfully connected to server.\n");

		AddSocket(m_pClient);

		setRunning(true);
		RunThreads();
	}

	void Client::disconnect()
	{
		if (!isRunning())
		{
			throw SocketException("Can't disconnect client when not connected.", 0);
		}

		setRunning(false);

		WaitEndOfThreads();
		CloseAllSockets();
	}

	void Client::send(std::string _msg)
	{
		if (isRunning())
		{
			AddMsgToSendQueue(m_pClient, _msg);
		}
	}
} //namespace net
