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
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>
#include "Socket.h"
#include "Event.h"
#include "Types.h"

using namespace std;

namespace net{

	class Client
	{
	private:
		Socket* m_sock;
		queue<string> m_toSend;
		queue<string> m_recvFrom;
		mutex m_mutToSendQueue;
		mutex m_mutRecvFromQueue;

		thread m_threadToSend;
		thread m_threadRecvFrom;

		std::atomic<bool> m_running;

	public:
		Client();
		Client(const Client& _c);

		~Client();

		void connect(string _addr, unsigned short _port, Socket::Protocol _protocol);
		void shutdown();

		template<typename T> void send(T& msg);

		// Return true and fill msg by first recieved message if any
		// If no recieved message then return false and don't modify msg
		template<typename T> bool pollMsg(T& _data);

		// Block execution until a msg is recv from server
		template<typename T> void waitMsg(T& msg);

	private:
		void RunSendToServer();
		void RunRecvFromServer();
	};
}

#include "Client.inl"

#endif // _CLIENT_H