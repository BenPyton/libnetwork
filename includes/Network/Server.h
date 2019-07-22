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

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>
#include "Event.h"
#include "Exports.h"
#include "Utils.h"
#include "Network/Serializer.h"

using namespace std;

namespace net{

	class Socket;


	class LIBNETWORK_API Server
	{
	private:
		struct Message {
			Socket* client;
			string msg;
		};
		Socket* m_pServer = nullptr;
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
		Server(const Server& _s) = delete;

		~Server();

		size_t nbClient();

		void launch(string _addr, unsigned short _port, Protocol _protocol);
		void shutdown();

		// Return true and fill _data by first received message if any
		// If no received message then return false and don't modify msg
		template<typename T> bool pollMsg(T& _data, Socket*& _client);

		// Block execution until a msg is recv from client, or any client if nullptr
		template<typename T> void waitMsg(T& _data, Socket* _client = nullptr);

		template<typename T> void send(class Socket* _client, T& _data);
		template<typename T> void broadcast(T& _data);

		Event<Socket*>& OnClientConnection() { return m_clientConnectionEvent; }
		Event<Socket*>& OnClientDisconnection() { return m_clientDisconnectionEvent; }

	private:
		void RunSendToClient(Socket* client);
		void RunRecvFromClient(Socket* client);
		void RunAcceptClient();

		void AddMsgToSendingQueue(const Message& _msg);
		bool GetMsgFromSendingQueue(Message& _msg, Socket* _client = nullptr);

		void AddMsgToReceivingQueue(const Message& _msg);
		bool GetMsgFromReceivingQueue(Message& _msg, Socket* _client = nullptr);

		bool _ClientIsRunning(Socket* _client);
	};
}

#include "Server.inl"

#endif // _SERVER_H