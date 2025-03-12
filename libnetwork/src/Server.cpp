/*
 * @author PELLETIER Benoit
 *
 * @file Server.cpp
 *
 * @date 17/07/2019
 *
 * @brief Define a server class, communicating with multiple clients
 *
 */

#include "Network/Socket.h"
#include "Network/Message.h"
#include "Network/Server.h"
#include "Network/SocketException.h"

namespace net
{
	Server::Server()
		: MsgSystem()
	{
	}

	Server::~Server()
	{
		if (m_running)
		{
			shutdown();
		}
	}

	void Server::launch(string _addr, unsigned short _port)
	{
		if (m_running)
		{
			throw new exception("Can't launch server when already running.");
		}

		m_pServer = new Socket(_addr, _port, Protocol::TCP);
		m_pServer->Bind();
		m_pServer->Listen();

		m_running = true;
		RunThreads();
		m_threadServerAccept = thread(&Server::_RunAcceptClient, this);
	}

	void Server::shutdown()
	{
		if (!m_running)
		{
			throw exception("Can't stop server when not running.");
		}

		m_running = false;

		// wait for threads to terminate
		m_threadServerAccept.join();
		WaitEndOfThreads();
		CloseAllSockets();

		// close and delete server socket
		m_pServer->Close();
		delete m_pServer;
	}

	void Server::send(Socket* _client, string _msg)
	{
		if (m_running)
		{
			AddMsgToSendQueue(_client, _msg);
		}
	}

	void Server::broadcast(string _msg)
	{
		send(NULL, _msg);
	}

	void Server::_RunAcceptClient()
	{
		fd_set acceptfds;
		timeval timeout;

		DebugLog("[Thread AcceptClient] Thread started !\n");
		while (m_running)
		{
			//clear the socket set
			FD_ZERO(&acceptfds);

			//add master socket to set
			FD_SET(m_pServer->GetSock(), &acceptfds);

			timeout.tv_sec = 0;
			timeout.tv_usec = 200 * 1000;
			if (select(m_pServer->GetSock() + 1, &acceptfds, NULL, NULL, &timeout) < 0) // update sockets in file descriptor
			{
				DebugLog("\n[Thread AcceptClient] Error during select.\n");
			}

			if (FD_ISSET(m_pServer->GetSock(), &acceptfds))
			{
				DebugLog("[Thread AcceptClient] Accepting incoming client connection...\n");
				Socket* sock = new Socket(m_pServer->Accept());

				AddSocket(sock);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		DebugLog("[Thread AcceptClient] Thread terminated !\n");
	}
} //namespace net
