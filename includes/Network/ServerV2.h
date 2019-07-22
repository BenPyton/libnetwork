/*
* @author PELLETIER Benoit
*
* @file ServerV2.h
*
* @date 17/07/2019
*
* @brief Define a server class, communicating with multiple clients
*
*/

#ifndef _SERVERV2_H
#define _SERVERV2_H

#include "Exports.h"
#include "MsgSystem.h"

using namespace std;

namespace net{

	class Socket;

	class LIBNETWORK_API ServerV2 : public MsgSystem
	{
	private:
		Socket* m_pServer = nullptr;

		thread m_threadServerAccept;

		//Event<Socket*> m_clientConnectionEvent;

	public:
		ServerV2();
		~ServerV2();

		void launch(string _addr, unsigned short _port);
		void shutdown();

		void send(Socket* _client, string _msg);
		void broadcast(string _data);

		//Event<Socket*>& onClientConnection() { return m_clientConnectionEvent; }

	private:
		void _RunAcceptClient();
	};
}

#endif // _SERVERV2_H