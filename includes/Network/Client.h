/*
* @author PELLETIER Benoit
*
* @file Client.h
*
* @date 18/11/2018
*
* @brief Define a client that can be connected to a server
*
*/

#ifndef _CLIENT_H
#define _CLIENT_H

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include "Exports.h"
#include "Network/Serializer.h"
#include "Utils.h"

using namespace std;

namespace net{
	class Socket;
	enum class Protocol;

	class LIBNETWORK_API Client
	{
	private:
		Socket* m_pSock;
		queue<string> m_toSend;
		queue<string> m_recvFrom;
		mutex m_mutToSendQueue;
		mutex m_mutRecvFromQueue;

		thread m_threadToSend;
		thread m_threadRecvFrom;

		std::atomic<bool> m_running;

	public:
		Client();
		Client(const Client& _c) = delete;

		~Client();

		void connect(string _addr, unsigned short _port, Protocol _protocol);
		void shutdown();

		template<typename T> void send(T& msg);

		// Return true and fill msg by first received message if any
		// If no received message then return false and don't modify msg
		template<typename T> bool pollMsg(T& _data);

		// Block execution until a msg is recv from server
		template<typename T> void waitMsg(T& msg);

	private:
		void RunSendToServer();
		void RunRecvFromServer();

		void AddMsgToSendingQueue(const string& _msg);
		bool GetMsgFromSendingQueue(string& _msg);

		void AddMsgToReceivingQueue(const string& _msg);
		bool GetMsgFromReceivingQueue(string& _msg);
	};
}

#include "Client.inl"

#endif // _CLIENT_H