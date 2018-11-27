/*
* @author PELLETIER Benoit
*
* @file Server.h
*
* @date 18/11/2018
*
* @brief Define server that can be connected with multiple clients
*
*/

#ifndef _SERVER_H
#define _SERVER_H

//#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>
#include "Socket.h"
#include "Event.h"
#include "Exports.h"
#include "Utils.h"
#include "Network/BinSerializer.h"

using namespace std;

namespace net{

	struct Message {
		Socket* client;
		string msg;
	};

	class LIBNETWORK_API Server
	{
	private:
		Socket* m_server = nullptr;
		vector<Socket*> m_clients; // Store client socket pointers on dynamic allocated memory
		mutex m_mutClientVector;

		queue<Message> m_toSend;
		queue<Message> m_recvFrom;
		mutex m_mutToSendQueue;
		mutex m_mutRecvFromQueue;

		Event<Socket*> m_clientConnectionEvent;
		Event<Socket*> m_clientDisconnectionEvent;

		thread m_threadServerAccept;

		std::atomic<bool> m_running = false;

	public:
		Server();
		Server(const Server& _s);

		~Server();

		size_t nbClient();

		void launch(wstring _addr, unsigned short _port, Socket::Protocol _protocol);
		void shutdown();

		// Return true and fill _data by first received message if any
		// If no received message then return false and don't modify msg
		template<typename T> bool pollMsg(T& _data, Socket*& _client);

		// Block execution until a msg is recv from client, or any client if nullptr
		template<typename T> void waitMsg(T& _data, Socket* _client = nullptr);

		template<typename T> void send(Socket* _client, T& _data);
		template<typename T> void broadcast(T& _data);

		Event<Socket*>& OnClientConnection() { return m_clientConnectionEvent; }
		Event<Socket*>& OnClientDisconnection() { return m_clientDisconnectionEvent; }

	private:
		void RunSendToClient(Socket* client);
		void RunRecvFromClient(Socket* client);
		void RunAcceptClient();

		bool _ClientIsRunning(Socket* _client);
	};
}

#include "Server.inl"

#endif // _SERVER_H