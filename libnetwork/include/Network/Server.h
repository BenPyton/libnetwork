/*
 * @author PELLETIER Benoit
 *
 * @file Server.h
 *
 * @date 17/07/2019
 *
 * @brief Define a server class, communicating with multiple clients
 *
 */

#ifndef _SERVER_H
#define _SERVER_H

#include "Exports.h"
#include "MsgSystem.h"

using namespace std;

namespace net
{

	class Socket;

	class LIBNETWORK_API Server : public MsgSystem
	{
	private:
		Socket* m_pServer = nullptr;

		thread m_threadServerAccept;

		//Event<Socket*> m_clientConnectionEvent;

	public:
		Server();
		~Server();

		void launch(string _addr, unsigned short _port);
		void shutdown();

		void send(Socket* _client, string _msg);
		void broadcast(string _data);

		//Event<Socket*>& onClientConnection() { return m_clientConnectionEvent; }

	private:
		void _RunAcceptClient();
	};
} //namespace net

#endif // _SERVERV2_H
